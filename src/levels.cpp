#include "levels.h"

#include <string>
#include <fstream>
#include <vector>

#include <cassert>
#include <cstring>

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>
// #include <PxPhysics.h>
// #include <PxPhysicsAPI.h>

#include "logging.h"
#include "util.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "asset_import/materials.h"
#include "ECS/Scene/Scene.h"
#include "ECS/Entity/CameraEntity.h"
#include "ECS/Entity/ModelEntity.h"
#include "ECS/Entity/SkeletalMeshEntity.h"
#include "ECS/Component/BaseComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/Component/AudioSourceComponent.h"
#include "ECS/Component/ModelComponent.h"
#include "ECS/Component/PlayerControllerComponent.h"
#include "ECS/Component/SkyBoxComponent.h"

static std::string currentLevelPath;
static std::string defaultLevelPath;
static Scene* currentScene;

glm::vec3 getVec3(rapidjson::Value& in) {
    glm::vec3 ret;
    ret.x = in[0].GetFloat();
    ret.y = in[1].GetFloat();
    ret.z = in[2].GetFloat();
    return ret;
}

// load level from manifest in path
void loadLevel(const char* path, Scene& scene) {
    // load document
    std::ifstream file(path);
    if (!file.is_open()) {
        logging::logErr("Failed to open file {} for reading\n", path);
        return;
    }

    // dump file contents to string
    file.seekg(0, file.end);
    size_t bufSize = size_t(file.tellg());
    file.seekg(0, file.beg);
    std::vector<char> buf(bufSize + 1);
    setVec(buf, '\0');
    file.read(buf.data(), bufSize);
    file.close();

    rapidjson::Document doc;
    doc.Parse(buf.data());
    if (doc.HasParseError()) {
        logging::logErr("JSON parse error: {}\n", doc.GetParseError());
        return;
    }
    // TODO replace asserts with error reporting
    assert(doc.IsObject());

    // clear all assets
    audio::clearAudio();
    clearDynamicTextures();
    scene.entities.clear();
    scene.uuidToIdx.clear();
    scene.selectedEntity = nullptr;

    std::string projRoot = assetfolder::getProjectRoot();
    if (!projRoot.empty())
        projRoot += "/";

    // load audio
    // store set of shared pointers temporarily before components are initialized
    std::vector<std::shared_ptr<audio::AudioDescriptor>> audioPtrs;
    {
        assert(doc.HasMember("audio"));
        auto jsonAudio = doc["audio"].GetArray();
        for (unsigned int i = 0; i < jsonAudio.Size(); i++) {
            audioPtrs.emplace_back(
                audio::audioLoad((projRoot + jsonAudio[i]["path"].GetString()).c_str(),
                    jsonAudio[i]["uuid"].GetString()));
        }
    }

    // load textures
    std::vector<std::shared_ptr<TextureDescriptor>> texturePtrs;
    {
        assert(doc.HasMember("textures"));
        auto jsonTextures = doc["textures"].GetArray();
        for (unsigned int i = 0; i < jsonTextures.Size(); i++) {
            texturePtrs.emplace_back(
                loadTexture((projRoot + jsonTextures[i]["path"].GetString()).c_str(),
                    jsonTextures[i]["uuid"].GetString()));
        }
    }

    MaterialSystem* matInstance = MaterialSystem::getInstance();
    // load materials
    {
        assert(doc.HasMember("materials"));
        auto jsonMaterials = doc["materials"].GetArray();
        for (unsigned int i = 0; i < jsonMaterials.Size(); i++) {
            Material mat;
            mat.name = jsonMaterials[i]["name"].GetString();
            mat.baseColor = getVec3(jsonMaterials[i]["baseColor"]);
            mat.emissiveColor = getVec3(jsonMaterials[i]["emissiveColor"]);

            mat.roughness = jsonMaterials[i]["roughness"].GetFloat();
            mat.metalness = jsonMaterials[i]["metalness"].GetFloat();
            mat.occlusion = jsonMaterials[i]["occlusion"].GetFloat();

            mat.baseColorMap = jsonMaterials[i]["baseColorMap"].GetString();
            mat.roughnessMap = jsonMaterials[i]["roughnessMap"].GetString();
            mat.metalnessMap = jsonMaterials[i]["metalnessMap"].GetString();
            mat.normalMap = jsonMaterials[i]["normalMap"].GetString();
            mat.alphaMap = jsonMaterials[i]["alphaMap"].GetString();
            mat.emissiveMap = jsonMaterials[i]["emissiveMap"].GetString();
            mat.occlusionMap = jsonMaterials[i]["occlusionMap"].GetString();

            matInstance->createMaterialDirectly(mat);
        }
    }

    // load models
    std::vector<std::shared_ptr<model::ModelDescriptor>> modelPtrs;
    {
        assert(doc.HasMember("models"));
        auto jsonModels = doc["models"].GetArray();
        for (unsigned int i = 0; i < jsonModels.Size(); i++) {
            modelPtrs.emplace_back(
                model::modelLoad((projRoot + jsonModels[i]["path"].GetString()).c_str(),
                    jsonModels[i]["uuid"].GetString()));
        }
    }

    // load animations
    std::vector<std::shared_ptr<animation::AnimationDescriptor>> animationPtrs;
    {
        assert(doc.HasMember("animations"));
        auto jsonModels = doc["animations"].GetArray();
        for (unsigned int i = 0; i < jsonModels.Size(); i++) {
            animationPtrs.emplace_back(
                animation::animationLoad((projRoot + jsonModels[i]["path"].GetString()).c_str(),
                    jsonModels[i]["uuid"].GetString(), model::modelGetByUuid(jsonModels[i]["modelUuid"].GetString())));
        }
    }

    // TODO load misc
    {
        assert(doc.HasMember("misc"));
        auto jsonMisc = doc["misc"].GetArray();
    }

    // load ECS
    {
        assert(doc.HasMember("entities"));
        auto jsonEntities = doc["entities"].GetArray();
        for (unsigned int i = 0; i < jsonEntities.Size(); i++) {
            auto entity = jsonEntities[i].GetObject();

            // add entity
            BaseEntity baseEntity(std::string(entity["name"].GetString()), entity["uuid"].GetInt());

            baseEntity.parent = entity["parent"].GetInt();
            baseEntity.state.position = glm::vec3(entity["position"][0].GetFloat(),
                entity["position"][1].GetFloat(), entity["position"][2].GetFloat());
            baseEntity.state.rotation =
                glm::quat(entity["rotation"][0].GetFloat(), entity["rotation"][1].GetFloat(),
                    entity["rotation"][2].GetFloat(), entity["rotation"][3].GetFloat());
            baseEntity.state.scale = glm::vec3(entity["scale"][0].GetFloat(),
                entity["scale"][1].GetFloat(), entity["scale"][2].GetFloat());

            // add components
            auto jsonComponents = entity["components"].GetArray();
            for (unsigned int j = 0; j < jsonComponents.Size(); j++) {
                auto jsonComponent = jsonComponents[j].GetObject();

                std::string name = std::string(jsonComponent["name"].GetString());
                int uuid = jsonComponent["uuid"].GetInt();

                // TransformComponent
                if (strcmp(jsonComponent["type"].GetString(), "TransformComponent") == 0) {
                    TransformComponent trComponent(name, uuid);

                    trComponent.position = glm::vec3(jsonComponent["position"][0].GetFloat(),
                        jsonComponent["position"][1].GetFloat(),
                        jsonComponent["position"][2].GetFloat());

                    trComponent.rotation = glm::quat(jsonComponent["rotation"][0].GetFloat(),
                        jsonComponent["rotation"][1].GetFloat(),
                        jsonComponent["rotation"][2].GetFloat(),
                        jsonComponent["rotation"][3].GetFloat());

                    trComponent.scale = glm::vec3(jsonComponent["scale"][0].GetFloat(),
                        jsonComponent["scale"][1].GetFloat(), jsonComponent["scale"][2].GetFloat());

                    baseEntity.components.addComponent(trComponent);
                }

                // ModelComponent
                else if (strcmp(jsonComponent["type"].GetString(), "ModelComponent") == 0) {
                    ModelComponent model(name, uuid);

                    model.modelUuid = std::string(jsonComponent["modelUuid"].GetString());
                    model.modelDescriptor = model::modelGetByUuid(model.modelUuid);

                    model.materials.resize(jsonComponent["materials"].Size());
                    for (unsigned int material = 0; material < jsonComponent["materials"].Size(); material++) {
                        model.materials[material] = matInstance->getActiveMaterial(jsonComponent["materials"][material].GetString());
                    }

                    baseEntity.components.addComponent(model);
                }

                // SkeletalModelComponent
                else if (strcmp(jsonComponent["type"].GetString(), "SkeletalModelComponent") == 0) {
                    SkeletalModelComponent model(name, uuid);

                    model.modelUuid = std::string(jsonComponent["modelUuid"].GetString());
                    model.modelDescriptor = model::modelGetByUuid(model.modelUuid);

                    model.materials.resize(jsonComponent["materials"].Size());
                    for (unsigned int material = 0; material < jsonComponent["materials"].Size(); material++) {
                        model.materials[material] = matInstance->getActiveMaterial(jsonComponent["materials"][material].GetString());
                    }

                    model.nodes.resize(jsonComponent["nodes"].Size());
                    for (unsigned int node = 0; node < jsonComponent["nodes"].Size(); node++) {
                        //read nodes without transitions
                        AnimationControllerNode currNode;
                        currNode.name = jsonComponent["nodes"][node]["name"].GetString();
                        currNode.animationUuid = jsonComponent["nodes"][node]["animationUuid"].GetString();
                        currNode.animationDescriptor = animation::animationGetByUuid(currNode.animationUuid);
                        currNode.loopCount = jsonComponent["nodes"][node]["loopCount"].GetInt();
                        model.nodes[node] = currNode;

                        model.nodes[node].noConditionTransitions.resize(jsonComponent["nodes"][node]["noconditiontransitions"].Size());
                        for (unsigned int noCond = 0; noCond < jsonComponent["nodes"][node]["noconditiontransitions"].Size(); noCond++) {
                            NoConditionACTransition noCondTr;
                            noCondTr.transitionTo = jsonComponent["nodes"][node]["noconditiontransitions"][noCond]["transitionTo"].GetString();
                            noCondTr.blendTime = jsonComponent["nodes"][node]["noconditiontransitions"][noCond]["blendTime"].GetDouble();
                            model.nodes[node].noConditionTransitions[noCond] = noCondTr;
                        }

                        model.nodes[node].boolTransitions.resize(jsonComponent["nodes"][node]["booltransitions"].Size());
                        for (unsigned int boolCond = 0; boolCond < jsonComponent["nodes"][node]["booltransitions"].Size(); boolCond++) {
                            BoolACTransition boolCondTr;
                            boolCondTr.transitionTo = jsonComponent["nodes"][node]["booltransitions"][boolCond]["transitionTo"].GetString();
                            boolCondTr.blendTime = jsonComponent["nodes"][node]["booltransitions"][boolCond]["blendTime"].GetDouble();
                            boolCondTr.immediate = jsonComponent["nodes"][node]["booltransitions"][boolCond]["immediate"].GetBool();
                            boolCondTr.condition = jsonComponent["nodes"][node]["booltransitions"][boolCond]["condition"].GetBool();
                            boolCondTr.desiredValue = jsonComponent["nodes"][node]["booltransitions"][boolCond]["desiredValue"].GetBool();
                            model.nodes[node].boolTransitions[boolCond] = boolCondTr;
                        }

                        model.nodes[node].intTransitions.resize(jsonComponent["nodes"][node]["inttransitions"].Size());
                        for (unsigned int intCond = 0; intCond < jsonComponent["nodes"][node]["inttransitions"].Size(); intCond++) {
                            IntACTransition intCondTr;
                            intCondTr.transitionTo = jsonComponent["nodes"][node]["inttransitions"][intCond]["transitionTo"].GetString();
                            intCondTr.blendTime = jsonComponent["nodes"][node]["inttransitions"][intCond]["blendTime"].GetDouble();
                            intCondTr.immediate = jsonComponent["nodes"][node]["inttransitions"][intCond]["immediate"].GetBool();
                            intCondTr.condition = jsonComponent["nodes"][node]["inttransitions"][intCond]["condition"].GetInt();
                            intCondTr.desiredValue = jsonComponent["nodes"][node]["inttransitions"][intCond]["desiredValue"].GetInt();
                            intCondTr.shouldBeLower = jsonComponent["nodes"][node]["inttransitions"][intCond]["shouldBeLower"].GetBool();
                            intCondTr.shouldBeEqual = jsonComponent["nodes"][node]["inttransitions"][intCond]["shouldBeEqual"].GetBool();
                            intCondTr.shouldBeGreater = jsonComponent["nodes"][node]["inttransitions"][intCond]["shouldBeGreater"].GetBool();
                            model.nodes[node].intTransitions[intCond] = intCondTr;
                        }

                        model.nodes[node].floatTransitions.resize(jsonComponent["nodes"][node]["floattransitions"].Size());
                        for (unsigned int floatCond = 0; floatCond < jsonComponent["nodes"][node]["floattransitions"].Size(); floatCond++) {
                            FloatACTransition floatCondTr;
                            floatCondTr.transitionTo = jsonComponent["nodes"][node]["floattransitions"][floatCond]["transitionTo"].GetString();
                            floatCondTr.blendTime = jsonComponent["nodes"][node]["floattransitions"][floatCond]["blendTime"].GetDouble();
                            floatCondTr.immediate = jsonComponent["nodes"][node]["floattransitions"][floatCond]["immediate"].GetBool();
                            floatCondTr.condition = jsonComponent["nodes"][node]["floattransitions"][floatCond]["condition"].GetDouble();
                            floatCondTr.desiredValue = jsonComponent["nodes"][node]["floattransitions"][floatCond]["desiredValue"].GetDouble();
                            floatCondTr.shouldBeLower = jsonComponent["nodes"][node]["floattransitions"][floatCond]["shouldBeLower"].GetBool();
                            floatCondTr.shouldBeEqual = jsonComponent["nodes"][node]["floattransitions"][floatCond]["shouldBeEqual"].GetBool();
                            floatCondTr.shouldBeGreater = jsonComponent["nodes"][node]["floattransitions"][floatCond]["shouldBeGreater"].GetBool();
                            model.nodes[node].floatTransitions[floatCond] = floatCondTr;
                        }
                    }

                    baseEntity.components.addComponent(model);
                }

                // SkyBoxComponent
                else if (strcmp(jsonComponent["type"].GetString(), "SkyBoxComponent") == 0) {
                    SkyBoxComponent sk(name, uuid);

                    sk.updateTex(0, jsonComponent["right"].GetString());
                    sk.updateTex(1, jsonComponent["left"].GetString());
                    sk.updateTex(2, jsonComponent["top"].GetString());
                    sk.updateTex(3, jsonComponent["bottom"].GetString());
                    sk.updateTex(4, jsonComponent["back"].GetString());
                    sk.updateTex(5, jsonComponent["front"].GetString());

                    baseEntity.components.addComponent(sk);
                }

                // AudioSourceComponent
                else if (strcmp(jsonComponent["type"].GetString(), "AudioSourceComponent") == 0) {
                    AudioSourceComponent audioSrc(name, uuid);

                    audioSrc.clipUuid = std::string(jsonComponent["clipUuid"].GetString());
                    audioSrc.clipDescriptor = audio::audioGetByUuid(audioSrc.clipUuid);
                    audioSrc.playOnStart = jsonComponent.HasMember("playOnStart")
                        ? jsonComponent["playOnStart"].GetBool()
                        : false;
                    audioSrc.loop = jsonComponent["loop"].GetBool();
                    audioSrc.directional = jsonComponent["directional"].GetBool();

                    baseEntity.components.addComponent(audioSrc);
                }

                // CameraComponent
                else if (strcmp(jsonComponent["type"].GetString(), "CameraComponent") == 0) {
                    CameraComponent cam(name, uuid);

                    cam.eye = glm::vec3(jsonComponent["eye"][0].GetFloat(),
                        jsonComponent["eye"][1].GetFloat(), jsonComponent["eye"][2].GetFloat());
                    cam.center = glm::vec3(jsonComponent["center"][0].GetFloat(),
                        jsonComponent["center"][1].GetFloat(),
                        jsonComponent["center"][2].GetFloat());
                    cam.up = glm::vec3(jsonComponent["up"][0].GetFloat(),
                        jsonComponent["up"][1].GetFloat(), jsonComponent["up"][2].GetFloat());
                    cam.fov = jsonComponent["fov"].GetFloat();
                    if (jsonComponent.HasMember("default") && jsonComponent["default"].GetBool()) {
                        CameraComponent::activeUuid = cam.uuid;
                    }
                    baseEntity.components.addComponent(cam);
                }

                // Light Component
                else if (strcmp(jsonComponent["type"].GetString(), "LightComponent") == 0) {
                    LightComponent light(name, uuid);

                    light.position = glm::vec3(jsonComponent["position"][0].GetFloat(),
                        jsonComponent["position"][1].GetFloat(),
                        jsonComponent["position"][2].GetFloat());
                    light.ambient = glm::vec3(jsonComponent["ambient"][0].GetFloat(),
                        jsonComponent["ambient"][1].GetFloat(),
                        jsonComponent["ambient"][2].GetFloat());
                    light.diffuse = glm::vec3(jsonComponent["diffuse"][0].GetFloat(),
                        jsonComponent["diffuse"][1].GetFloat(),
                        jsonComponent["diffuse"][2].GetFloat());
                    light.specular = glm::vec3(jsonComponent["specular"][0].GetFloat(),
                        jsonComponent["specular"][1].GetFloat(),
                        jsonComponent["specular"][2].GetFloat());
                    baseEntity.components.addComponent(light);
                }

                // ScriptComponent
                else if (strcmp(jsonComponent["type"].GetString(), "ScriptComponent") == 0) {
                    ScriptComponent script(name, uuid);
                    script.scriptPath = projRoot + jsonComponent["scriptPath"].GetString();

                    // handle args
                    auto scriptArgs = jsonComponent["args"].GetArray();
                    for (size_t arg = 0; arg < scriptArgs.Size(); arg++) {
                        auto jsonArg = scriptArgs[arg].GetObject();

                        ScriptArgument scriptArg;
                        scriptArg.type = (ScriptArgument::ArgType)jsonArg["type"].GetInt();
                        scriptArg.key = jsonArg["key"].GetString();
                        rapidjson::Value loc;
                        switch (scriptArg.type) {
                        case ScriptArgument::BOOL:
                        case ScriptArgument::INT:
                        case ScriptArgument::ENTITY:
                            scriptArg.arg._int = jsonArg["val"].GetInt();
                            break;
                        case ScriptArgument::FLOAT:
                            scriptArg.arg._float = jsonArg["val"].GetFloat();
                            break;
                        case ScriptArgument::STRING:
                            scriptArg.stringBuf = jsonArg["val"].GetString();
                            break;
                        case ScriptArgument::COMPONENT:
                            loc = jsonArg["val"].GetObject();
                            scriptArg.arg._loc.componentIdx = loc["componentIdx"].GetInt();
                            scriptArg.arg._loc.entityUuid = loc["entityUuid"].GetInt();
                            scriptArg.arg._loc.type =
                                (ComponentLocation::CompType)loc["type"].GetInt();
                            break;
                        default:;
                        }
                        script.args.emplace_back(scriptArg);
                    }

                    baseEntity.components.addComponent(script);
                }
                // PlayerControllerComponent
                else if (strcmp(jsonComponent["type"].GetString(), "PlayerControllerComponent") == 0) {
                    PlayerControllerComponent controls;
                    controls.uuid = jsonComponent["uuid"].GetInt();
                    controls.name = jsonComponent["name"].GetString();
                    auto const virtualKeys = jsonComponent["virtualKeys"].GetArray();
                    for (int j = 0; j < virtualKeys.Size(); j++) {
                        controls.addKey();
                        controls.virtualKeys[j].name = virtualKeys[j]["name"].GetString();
                        controls.virtualKeys[j].key = virtualKeys[j]["Key"].GetInt();
                        controls.virtualKeys[j].scale = virtualKeys[j]["scale"].GetFloat();
                        controls.virtualKeys[j].action = virtualKeys[j]["action"].GetInt();
                    }
                    baseEntity.components.addComponent(controls);
                }
                // BaseComponent
                else {
                    BaseComponent base(name, uuid);

                    baseEntity.components.addComponent(base);
                }


            }
            scene.addEntity(baseEntity);
        }
    }

    // perform initial update on scene
    scene.updatePositions();
    for (size_t i = 0; i < scene.entities.size(); i++) {
        for (size_t j = 0; j < scene.entities[i].components.vecLightComponent.size(); j++) {
            scene.entities[i].components.vecLightComponent[j].update(.0f, scene.entities[i].state);
        }
    }

    // set level to current if everything is succesful
    currentLevelPath = path;
    currentScene = &scene;
    logging::logInfo("Opened level {}\n", path);
}
static std::string enqueuedLevel;
static int luaEnqueueLevel(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 1) {
        return 0;
    }
    enqueuedLevel = std::string(luaL_tolstring(state, 1, nullptr));
    return 0;
}
void registerLevelFunctionsLua() {
    scripting::beginModule(1);
    scripting::registerModuleFunction("enqueueLevel", &luaEnqueueLevel);
    scripting::finalizeModule("levels");
}

void updateLevels(Scene& scene) {
    if (!enqueuedLevel.empty()) {
        loadLevel(enqueuedLevel.c_str(), scene);
        enqueuedLevel = "";
    }
}

#ifdef ONO_ENGINE_ONLY
// --- Per-Component-Type serialization functions
static void saveComponent(const TransformComponent& trComponent,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {

    writer.StartObject();

    // shared properties
    writer.Key("name");
    writer.String(trComponent.name.c_str());

    writer.Key("uuid");
    writer.Int(trComponent.uuid);

    writer.Key("type");
    writer.String("TransformComponent");

    writer.Key("position");
    // position
    writer.StartArray();
    writer.Double(double(trComponent.position[0]));
    writer.Double(double(trComponent.position[1]));
    writer.Double(double(trComponent.position[2]));
    writer.EndArray();
    // rotation
    writer.Key("rotation");
    writer.StartArray();
    writer.Double(trComponent.rotation[0]);
    writer.Double(trComponent.rotation[1]);
    writer.Double(trComponent.rotation[2]);
    writer.Double(trComponent.rotation[3]);

    writer.EndArray();

    // scale
    writer.Key("scale");
    writer.StartArray();
    writer.Double(double(trComponent.scale[0]));
    writer.Double(double(trComponent.scale[1]));
    writer.Double(double(trComponent.scale[2]));
    writer.EndArray();

    writer.EndObject();
}

static void saveComponent(const BaseComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();
    writer.Key("name");
    writer.String(component.name.c_str());
    writer.Key("uuid");
    writer.Int(component.uuid);
    writer.Key("type");
    writer.String("BaseComponent");
    writer.EndObject();
}

static void saveComponent(const AudioSourceComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("AudioSourceComponent");

    writer.Key("clipUuid");
    writer.String(component.clipUuid.c_str());

    writer.Key("loop");
    writer.Bool(component.loop);

    writer.Key("directional");
    writer.Bool(component.directional);

    writer.Key("playOnStart");
    writer.Bool(component.playOnStart);

    writer.EndObject();
}
static void saveComponent(const SkyBoxComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("SkyBoxComponent");

    writer.Key("right");
    writer.String(component.skybox.faces[0].path.c_str());

    writer.Key("left");
    writer.String(component.skybox.faces[1].path.c_str());

    writer.Key("top");
    writer.String(component.skybox.faces[2].path.c_str());

    writer.Key("bottom");
    writer.String(component.skybox.faces[3].path.c_str());

    writer.Key("back");
    writer.String(component.skybox.faces[4].path.c_str());

    writer.Key("front");
    writer.String(component.skybox.faces[5].path.c_str());

    writer.EndObject();
}
static void saveComponent(const ModelComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("ModelComponent");

    writer.Key("modelUuid");
    writer.String(component.modelUuid.c_str());

    writer.Key("materials");
    writer.StartArray();
    for (int i = 0; i < component.materials.size(); i++) {
        writer.String(component.materials[i]->name.c_str());
    }
    writer.EndArray();


    writer.EndObject();
}

static void saveComponent(const SkeletalModelComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("SkeletalModelComponent");

    writer.Key("modelUuid");
    writer.String(component.modelUuid.c_str());

    writer.Key("materials");
    writer.StartArray();
    for (int i = 0; i < component.materials.size(); i++) {
        writer.String(component.materials[i]->name.c_str());
    }
    writer.EndArray();

    writer.Key("nodes");
    writer.StartArray();
    for (int i = 0; i < component.nodes.size(); i++) {
        writer.StartObject();

        writer.Key("name");
        writer.String(component.nodes[i].name.c_str());

        writer.Key("animationUuid");
        writer.String(component.nodes[i].animationUuid.c_str());

        writer.Key("loopCount");
        writer.Int(component.nodes[i].loopCount);

        writer.Key("noconditiontransitions");
        writer.StartArray();
        for (int j = 0; j < component.nodes[i].noConditionTransitions.size(); j++) {
            writer.StartObject();

            writer.Key("transitionTo");
            writer.String(component.nodes[i].noConditionTransitions[j].transitionTo.c_str());

            writer.Key("blendTime");
            writer.Double(component.nodes[i].noConditionTransitions[j].blendTime);

            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("booltransitions");
        writer.StartArray();
        for (int j = 0; j < component.nodes[i].boolTransitions.size(); j++) {
            writer.StartObject();

            writer.Key("transitionTo");
            writer.String(component.nodes[i].boolTransitions[j].transitionTo.c_str());

            writer.Key("blendTime");
            writer.Double(component.nodes[i].boolTransitions[j].blendTime);

            writer.Key("immediate");
            writer.Bool(component.nodes[i].boolTransitions[j].immediate);

            writer.Key("condition");
            writer.Bool(component.nodes[i].boolTransitions[j].condition);

            writer.Key("desiredValue");
            writer.Bool(component.nodes[i].boolTransitions[j].desiredValue);

            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("inttransitions");
        writer.StartArray();
        for (int j = 0; j < component.nodes[i].intTransitions.size(); j++) {
            writer.StartObject();

            writer.Key("transitionTo");
            writer.String(component.nodes[i].intTransitions[j].transitionTo.c_str());

            writer.Key("blendTime");
            writer.Double(component.nodes[i].intTransitions[j].blendTime);

            writer.Key("immediate");
            writer.Bool(component.nodes[i].intTransitions[j].immediate);

            writer.Key("condition");
            writer.Int(component.nodes[i].intTransitions[j].condition);

            writer.Key("desiredValue");
            writer.Int(component.nodes[i].intTransitions[j].desiredValue);

            writer.Key("shouldBeLower");
            writer.Bool(component.nodes[i].intTransitions[j].shouldBeLower);

            writer.Key("shouldBeEqual");
            writer.Bool(component.nodes[i].intTransitions[j].shouldBeEqual);

            writer.Key("shouldBeGreater");
            writer.Bool(component.nodes[i].intTransitions[j].shouldBeGreater);

            writer.EndObject();
        }
        writer.EndArray();

        writer.Key("floattransitions");
        writer.StartArray();
        for (int j = 0; j < component.nodes[i].floatTransitions.size(); j++) {
            writer.StartObject();

            writer.Key("transitionTo");
            writer.String(component.nodes[i].floatTransitions[j].transitionTo.c_str());

            writer.Key("blendTime");
            writer.Double(component.nodes[i].floatTransitions[j].blendTime);

            writer.Key("immediate");
            writer.Bool(component.nodes[i].floatTransitions[j].immediate);

            writer.Key("condition");
            writer.Double(component.nodes[i].floatTransitions[j].condition);

            writer.Key("desiredValue");
            writer.Double(component.nodes[i].floatTransitions[j].desiredValue);

            writer.Key("shouldBeLower");
            writer.Bool(component.nodes[i].floatTransitions[j].shouldBeLower);

            writer.Key("shouldBeEqual");
            writer.Bool(component.nodes[i].floatTransitions[j].shouldBeEqual);

            writer.Key("shouldBeGreater");
            writer.Bool(component.nodes[i].floatTransitions[j].shouldBeGreater);

            writer.EndObject();
        }
        writer.EndArray();

        writer.EndObject();
    }
    writer.EndArray();


    writer.EndObject();
}

static void saveComponent(const CameraComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("CameraComponent");

    writer.Key("eye");
    writer.StartArray();
    writer.Double(float(component.eye[0]));
    writer.Double(float(component.eye[1]));
    writer.Double(float(component.eye[2]));
    writer.EndArray();

    writer.Key("center");
    writer.StartArray();
    writer.Double(float(component.center[0]));
    writer.Double(float(component.center[1]));
    writer.Double(float(component.center[2]));
    writer.EndArray();

    writer.Key("up");
    writer.StartArray();
    writer.Double(float(component.up[0]));
    writer.Double(float(component.up[1]));
    writer.Double(float(component.up[2]));
    writer.EndArray();

    writer.Key("fov");
    writer.Double(double(component.fov));

    if (CameraComponent::activeUuid == component.uuid) {
        writer.Key("default");
        writer.Bool(true);
    }

    writer.EndObject();
}

static void saveComponent(const LightComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("LightComponent");

    writer.Key("position");
    writer.StartArray();
    writer.Double(float(component.position[0]));
    writer.Double(float(component.position[1]));
    writer.Double(float(component.position[2]));
    writer.EndArray();

    writer.Key("ambient");
    writer.StartArray();
    writer.Double(float(component.ambient[0]));
    writer.Double(float(component.ambient[1]));
    writer.Double(float(component.ambient[2]));
    writer.EndArray();

    writer.Key("diffuse");
    writer.StartArray();
    writer.Double(float(component.diffuse[0]));
    writer.Double(float(component.diffuse[1]));
    writer.Double(float(component.diffuse[2]));
    writer.EndArray();

    writer.Key("specular");
    writer.StartArray();
    writer.Double(float(component.specular[0]));
    writer.Double(float(component.specular[1]));
    writer.Double(float(component.specular[2]));
    writer.EndArray();

    writer.EndObject();
}

static void saveComponent(const ScriptComponent& component,
    rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();
    writer.Key("name");
    writer.String(component.name.c_str());
    writer.Key("uuid");
    writer.Int(component.uuid);
    writer.Key("type");
    writer.String("ScriptComponent");
    writer.Key("scriptPath");
    writer.String(assetfolder::getRelativePath(component.scriptPath.c_str()).c_str());
    writer.Key("args");
    writer.StartArray();
    for (size_t i = 0; i < component.args.size(); i++) {
        writer.StartObject();
        writer.Key("key");
        writer.String(component.args[i].key.c_str());
        writer.Key("type");
        writer.Int(component.args[i].type);
        switch (component.args[i].type) {
        case ScriptArgument::BOOL:
        case ScriptArgument::INT:
        case ScriptArgument::ENTITY:
            writer.Key("val");
            writer.Int(component.args[i].arg._int);
            break;
        case ScriptArgument::FLOAT:
            writer.Key("val");
            writer.Double(component.args[i].arg._float);
            break;
        case ScriptArgument::STRING:
            writer.Key("val");
            writer.String(component.args[i].stringBuf.c_str());
            break;
        case ScriptArgument::COMPONENT:
            writer.Key("val");
            writer.StartObject();
            writer.Key("type");
            writer.Int(component.args[i].arg._loc.type);
            writer.Key("entityUuid");
            writer.Int(component.args[i].arg._loc.entityUuid);
            writer.Key("componentIdx");
            writer.Int(component.args[i].arg._loc.componentIdx);
            writer.EndObject();
            break;
        default:;
        }
    }
    writer.EndArray();
    writer.EndObject();
}

static void saveComponent(
    const PlayerControllerComponent& component, rapidjson::Writer<rapidjson::FileWriteStream>& writer) {
    writer.StartObject();

    writer.Key("name");
    writer.String(component.name.c_str());

    writer.Key("uuid");
    writer.Int(component.uuid);

    writer.Key("type");
    writer.String("PlayerControllerComponent");

    writer.Key("virtualKeys");
    writer.StartArray();
    for (auto const& vk : component.virtualKeys) {
        writer.StartObject();
        writer.Key("name");
        writer.String(vk.name.c_str());

        writer.Key("Key");
        writer.Int(vk.key);

        writer.Key("scale");
        writer.Double(double(vk.scale));

        writer.Key("action");
        writer.Int(vk.action);
        writer.EndObject();
    }
    writer.EndArray();
    writer.EndObject();
}

// save level to manifest in path
void saveLevel(const char* path, const Scene& scene) {
    // open document for writing
#pragma warning(disable : 4996)
    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        logging::logErr("Failed to open file {} for writing\n", path);
        return;
    }

    char writeBuf[BUFSIZ];
    rapidjson::FileWriteStream osw(file, writeBuf, BUFSIZ);
    rapidjson::Writer<rapidjson::FileWriteStream> writer(osw);

    writer.StartObject();

    // save ECS and enumerate dependencies
    std::vector<int> modelIds;
    std::vector<int> textureIds;
    std::vector<int> scriptIds;
    std::vector<int> miscIds;

    // encode entities
    {
        writer.Key("entities");
        writer.StartArray();
        // rapidjson::Value jsonEntities(rapidjson::kArrayType);
        for (unsigned int i = 0; i < scene.entities.size(); i++) {
            writer.StartObject();

            // encode entity properties
            writer.Key("name");
            writer.String(scene.entities[i].name.c_str());

            writer.Key("uuid");
            writer.Int(scene.entities[i].uuid);

            writer.Key("parent");
            writer.Int(scene.entities[i].parent);

            // position
            writer.Key("position");
            writer.StartArray();

            writer.Double(double(scene.entities[i].state.position[0]));
            writer.Double(double(scene.entities[i].state.position[1]));
            writer.Double(double(scene.entities[i].state.position[2]));

            writer.EndArray();

            // rotation
            writer.Key("rotation");
            writer.StartArray();

            writer.Double(double(scene.entities[i].state.rotation[0]));
            writer.Double(double(scene.entities[i].state.rotation[1]));
            writer.Double(double(scene.entities[i].state.rotation[2]));
            writer.Double(double(scene.entities[i].state.rotation[3]));

            writer.EndArray();

            // scale
            writer.Key("scale");
            writer.StartArray();

            writer.Double(double(scene.entities[i].state.scale[0]));
            writer.Double(double(scene.entities[i].state.scale[1]));
            writer.Double(double(scene.entities[i].state.scale[2]));

            writer.EndArray();

            // encode components
            writer.Key("components");
            writer.StartArray();

            // TransformComponent
            const std::vector<TransformComponent>& transformComponents =
                scene.entities[i].components.vecTransformComponent;
            for (unsigned int j = 0; j < transformComponents.size(); j++) {
                saveComponent(transformComponents[j], writer);
            }

            // AudioSourceComponent
            const std::vector<AudioSourceComponent>& audioSrcComponents =
                scene.entities[i].components.vecAudioSourceComponent;
            for (unsigned int j = 0; j < audioSrcComponents.size(); j++) {
                saveComponent(audioSrcComponents[j], writer);
            }

            // CameraComponent
            const std::vector<CameraComponent>& cameraComponents =
                scene.entities[i].components.vecCameraComponent;
            for (unsigned int j = 0; j < cameraComponents.size(); j++) {
                saveComponent(cameraComponents[j], writer);
            }

            // LightComponent
            const std::vector<LightComponent>& lightComponents =
                scene.entities[i].components.vecLightComponent;
            for (unsigned int j = 0; j < lightComponents.size(); j++) {
                saveComponent(lightComponents[j], writer);
            }

            // ModelComponent
            const std::vector<ModelComponent>& modelComponents =
                scene.entities[i].components.vecModelComponent;
            for (unsigned int j = 0; j < modelComponents.size(); j++) {
                saveComponent(modelComponents[j], writer);
            }

            // SkeletalModelComponent
            const std::vector<SkeletalModelComponent>& skeletalModelComponents =
                scene.entities[i].components.vecSkeletalModelComponent;
            for (unsigned int j = 0; j < skeletalModelComponents.size(); j++) {
                saveComponent(skeletalModelComponents[j], writer);
            }

            // SkyBoxComponent
            const std::vector<SkyBoxComponent>& skyboxComponents =
                scene.entities[i].components.vecSkyBoxComponent;
            for (unsigned int j = 0; j < skyboxComponents.size(); j++) {
                saveComponent(skyboxComponents[j], writer);
            }

            // ScriptComponent
            const std::vector<ScriptComponent>& scriptComponents =
                scene.entities[i].components.vecScriptComponent;
            for (size_t j = 0; j < scriptComponents.size(); j++) {
                saveComponent(scriptComponents[j], writer);
            }

            // BaseComponent
            const std::vector<BaseComponent>& baseComponents =
                scene.entities[i].components.vecBaseComponent;
            for (unsigned int j = 0; j < baseComponents.size(); j++) {
                saveComponent(baseComponents[j], writer);
            }

            // PlayerControllerComponent
            const std::vector<PlayerControllerComponent>& playerControllerComponents =
                scene.entities[i].components.vecPlayerControllerComponent;
            for (unsigned int j = 0; j < playerControllerComponents.size(); j++) {
                saveComponent(playerControllerComponents[j], writer);
            }

            writer.EndArray();
            writer.EndObject();
        }
        writer.EndArray();
    }

    // save audio
    {
        writer.Key("audio");
        writer.StartArray();
        std::vector<audio::AudioDiskData> data;
        audio::getDiskData(data);

        for (unsigned int i = 0; i < data.size(); i++) {
            writer.StartObject();

            writer.Key("uuid");
            writer.String(data[i].uuid.c_str());

            writer.Key("path");
            writer.String(assetfolder::getRelativePath(data[i].path.c_str()).c_str());

            writer.EndObject();
        }

        writer.EndArray();
    }

    // save materials
    {
        writer.Key("materials");
        writer.StartArray();
        MaterialSystem* matInstance = MaterialSystem::getInstance();
        for (auto const& mat : matInstance->materials) {
            writer.StartObject();

            writer.Key("name");
            writer.String(mat.second.name.c_str());

            writer.Key("baseColor");
            writer.StartArray();
            writer.Double(mat.second.baseColor.x);
            writer.Double(mat.second.baseColor.y);
            writer.Double(mat.second.baseColor.z);
            writer.EndArray();

            writer.Key("emissiveColor");
            writer.StartArray();
            writer.Double(mat.second.emissiveColor.x);
            writer.Double(mat.second.emissiveColor.y);
            writer.Double(mat.second.emissiveColor.z);
            writer.EndArray();

            writer.Key("roughness");
            writer.Double(mat.second.roughness);
            writer.Key("metalness");
            writer.Double(mat.second.metalness);
            writer.Key("occlusion");
            writer.Double(mat.second.occlusion);

            writer.Key("baseColorMap");
            writer.String(mat.second.baseColorMap.c_str());
            writer.Key("roughnessMap");
            writer.String(mat.second.roughnessMap.c_str());
            writer.Key("metalnessMap");
            writer.String(mat.second.metalnessMap.c_str());
            writer.Key("normalMap");
            writer.String(mat.second.normalMap.c_str());
            writer.Key("alphaMap");
            writer.String(mat.second.alphaMap.c_str());
            writer.Key("emissiveMap");
            writer.String(mat.second.emissiveMap.c_str());
            writer.Key("occlusionMap");
            writer.String(mat.second.occlusionMap.c_str());

            writer.EndObject();
        }
        writer.EndArray();

    }

    // TODO - save models
    {
        writer.Key("models");
        writer.StartArray();
        std::vector<model::ModelDiskData> data;
        model::getDiskData(data);

        for (unsigned int i = 0; i < data.size(); i++) {
            writer.StartObject();

            writer.Key("uuid");
            writer.String(data[i].uuid.c_str());

            writer.Key("path");
            writer.String(assetfolder::getRelativePath(data[i].path.c_str()).c_str());

            writer.EndObject();
        }

        writer.EndArray();
    }

    //save animations
    {
        writer.Key("animations");
        writer.StartArray();
        std::vector<animation::AnimationDiskData> data;
        animation::getDiskData(data);

        for (unsigned int i = 0; i < data.size(); i++) {
            writer.StartObject();

            writer.Key("uuid");
            writer.String(data[i].uuid.c_str());

            writer.Key("path");
            writer.String(assetfolder::getRelativePath(data[i].path.c_str()).c_str());

            writer.Key("modelUuid");
            writer.String(data[i].modelUuid.c_str());

            writer.EndObject();
        }

        writer.EndArray();
    }

    // TODO - save textures
    {
        writer.Key("textures");
        writer.StartArray();
        writer.EndArray();
    }

    // TODO - save scripts
    {
        writer.Key("scripts");
        writer.StartArray();
        writer.EndArray();
    }

    // TODO - save misc
    {
        writer.Key("misc");
        writer.StartArray();
        writer.EndArray();
    }

    // save and close document
    writer.EndObject();
    writer.Flush();
    osw.Flush();
    fclose(file);

    logging::logInfo("Saved level {}\n", path);
}

// save last loaded level
void saveCurrentLevel() {
    if (!currentLevelPath.empty())
        saveLevel(currentLevelPath.c_str(), *currentScene);
}
#endif

// load default project from project file
std::string loadProjectFile(const char* path) {
    // try to open file
    std::ifstream file(path);
    if (!file.is_open()) {
        logging::logErr("Failed to open file {} for reading\n", path);
        return std::string("");
    }

    // dump file contents to string
    file.seekg(0, file.end);
    size_t bufSize = size_t(file.tellg());
    file.seekg(0, file.beg);
    std::vector<char> buf(bufSize + 1);
    setVec(buf, '\0');
    file.read(buf.data(), bufSize);
    file.close();

    // parse document
    rapidjson::Document doc;
    doc.Parse(buf.data());
    if (doc.HasParseError()) {
        logging::logErr("JSON parse error: {}\n", doc.GetParseError());
        return std::string("");
    }

    assert(doc.HasMember("base_level"));
    logging::logInfo("Opened project file at: {}\n", path);

    defaultLevelPath = std::string(doc["base_level"].GetString());
    return defaultLevelPath;
}

#ifdef ONO_ENGINE_ONLY
// save project file to manifest
void saveProjectFile(const char* path) {
    if (defaultLevelPath.empty())
        return;

    FILE* file = fopen(path, "wb");
    if (file == NULL) {
        logging::logErr("Failed to open file {} for writing\n", path);
        return;
    }

    rapidjson::Document doc;
    doc.SetObject();
    doc.AddMember("base_level", rapidjson::Value(defaultLevelPath.c_str(), doc.GetAllocator()),
        doc.GetAllocator());

    char writeBuf[BUFSIZ];
    rapidjson::FileWriteStream osw(file, writeBuf, BUFSIZ);
    rapidjson::Writer<rapidjson::FileWriteStream> writer(osw);
    doc.Accept(writer);
    fclose(file);

    logging::logInfo("Saved project file at {}\n", path);
}

bool isDefaultLevel(const std::string& name) {
    return (name == assetfolder::getName(defaultLevelPath.c_str()));
}

void setDefaultLevel(const std::string& path) {
    defaultLevelPath = assetfolder::getRelativePath(path.c_str());
}

bool isCurrentLevel(const std::string& name) {
    return (name == assetfolder::getName(currentLevelPath.c_str()));
}
#endif
