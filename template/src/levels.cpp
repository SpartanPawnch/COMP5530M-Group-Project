#include "levels.h"

#include <string>
#include <fstream>
#include <vector>

#include <cassert>
#include <cstring>

#include <rapidjson/document.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson/writer.h>

#include "logging.h"
#include "util.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "ECS/Scene/Scene.h"
#include "ECS/Entity/CameraEntity.h"
#include "ECS/Entity/ModelEntity.h"
#include "ECS/Entity/SkeletalMeshEntity.h"
#include "ECS/Component/BaseComponent.h"
#include "ECS/Component/TransformComponent.h"
#include "ECS/Component/AudioSourceComponent.h"
#include "ECS/Component/ModelComponent.h"

static std::string currentLevelPath;
static std::string defaultLevelPath;
static Scene* currentScene;

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
            BaseEntity baseEntity;

            baseEntity.uuid = entity["uuid"].GetInt();
            baseEntity.name = std::string(entity["name"].GetString());
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

                // TransformComponent
                if (strcmp(jsonComponent["type"].GetString(), "TransformComponent") == 0) {
                    TransformComponent trComponent;
                    trComponent.position = glm::vec3(jsonComponent["position"][0].GetFloat(),
                        jsonComponent["position"][1].GetFloat(),
                        jsonComponent["position"][2].GetFloat());

                    trComponent.rotation = glm::quat(jsonComponent["rotation"][0].GetFloat(),
                        jsonComponent["rotation"][1].GetFloat(),
                        jsonComponent["rotation"][2].GetFloat(),
                        jsonComponent["rotation"][3].GetFloat());

                    trComponent.scale = glm::vec3(jsonComponent["scale"][0].GetFloat(),
                        jsonComponent["scale"][1].GetFloat(), jsonComponent["scale"][2].GetFloat());

                    trComponent.name = std::string(jsonComponent["name"].GetString());
                    trComponent.uuid = jsonComponent["uuid"].GetInt();

                    baseEntity.components.addComponent(trComponent);
                }

                // AudioSourceComponent
                else if (strcmp(jsonComponent["type"].GetString(), "AudioSourceComponent") == 0) {
                    AudioSourceComponent audioSrc;
                    audioSrc.uuid = jsonComponent["uuid"].GetInt();
                    audioSrc.name = std::string(jsonComponent["name"].GetString());
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
                    CameraComponent cam;
                    cam.uuid = jsonComponent["uuid"].GetInt();
                    cam.name = std::string(jsonComponent["name"].GetString());
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
                    LightComponent light;
                    light.uuid = jsonComponent["uuid"].GetInt();
                    light.name = std::string(jsonComponent["name"].GetString());
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

                // ModelComponent
                else if (strcmp(jsonComponent["type"].GetString(), "ModelComponent") == 0) {
                    ModelComponent model;
                    model.uuid = jsonComponent["uuid"].GetInt();
                    model.name = std::string(jsonComponent["name"].GetString());
                    model.modelUuid = std::string(jsonComponent["modelUuid"].GetString());
                    model.modelDescriptor = model::modelGetByUuid(model.modelUuid);
                    baseEntity.components.addComponent(model);
                }
                // ScriptComponent
                else if (strcmp(jsonComponent["type"].GetString(), "ScriptComponent") == 0) {
                    int uuid = jsonComponent["uuid"].GetInt();
                    std::string name = std::string(jsonComponent["name"].GetString());
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
                // BaseComponent
                else {
                    BaseComponent base;
                    base.name = std::string(jsonComponent["name"].GetString());
                    base.uuid = jsonComponent["uuid"].GetInt();

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
