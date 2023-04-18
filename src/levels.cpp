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
    scripting::clearScripts();
    scene.entities.clear();
    scene.selectedEntity = nullptr;

    // load audio
    // store set of shared pointers temporarily before components are initialized
    std::vector<std::shared_ptr<audio::AudioDescriptor>> audioPtrs;
    {
        assert(doc.HasMember("audio"));
        auto jsonAudio = doc["audio"].GetArray();
        for (unsigned int i = 0; i < jsonAudio.Size(); i++) {
            audioPtrs.emplace_back(audio::audioLoad(
                jsonAudio[i]["path"].GetString(), jsonAudio[i]["uuid"].GetString()));
        }
    }

    // load textures
    {
        assert(doc.HasMember("textures"));
        auto jsonTextures = doc["textures"].GetArray();
        for (unsigned int i = 0; i < jsonTextures.Size(); i++) {
            // TODO init with fixed uuid
            // loadTexture(jsonTextures[i]["path"].GetString());
        }
    }

    // TODO - load models
    {
        assert(doc.HasMember("models"));
        auto jsonModels = doc["models"].GetArray();
    }

    // TODO load scripts
    {
        assert(doc.HasMember("scripts"));
        auto jsonScripts = doc["scripts"].GetArray();
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
            // TODO - currently ignoring type, need to think about component only vs entity params
            BaseEntity baseEntity;

            baseEntity.uuid = entity["uuid"].GetInt();
            baseEntity.name = std::string(entity["name"].GetString());
            baseEntity.parent = entity["parent"].GetInt();
            baseEntity.position = glm::vec3(entity["position"][0].GetFloat(),
                entity["position"][1].GetFloat(), entity["position"][2].GetFloat());
            baseEntity.rotation =
                glm::quat(entity["rotation"][0].GetFloat(), entity["rotation"][1].GetFloat(),
                    entity["rotation"][2].GetFloat(), entity["rotation"][3].GetFloat());
            baseEntity.scale = glm::vec3(entity["scale"][0].GetFloat(),
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

                    trComponent.rotation = glm::vec4(jsonComponent["rotation"][0].GetFloat(),
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
                    audioSrc.loop = jsonComponent["loop"].GetBool();
                    audioSrc.directional = jsonComponent["directional"].GetBool();

                    baseEntity.components.addComponent(audioSrc);
                }

                // BaseComponent
                else {
                    BaseComponent base;
                    base.name = std::string(jsonComponent["name"].GetString());
                    base.uuid = jsonComponent["uuid"].GetInt();

                    baseEntity.components.addComponent(base);
                }
            }
            scene.entities.emplace_back(baseEntity);
        }
    }

    // set level to current if everything is succesful
    currentLevelPath = path;
    currentScene = &scene;
    logging::logInfo("Opened level {}\n", path);
}

// --- Per-Component-Type serialization functions
static rapidjson::Value saveComponent(
    const TransformComponent& trComponent, rapidjson::Document& d) {
    rapidjson::Value jsonComponent(rapidjson::kObjectType);

    jsonComponent.AddMember(
        "name", rapidjson::Value(trComponent.name.c_str(), d.GetAllocator()), d.GetAllocator());
    jsonComponent.AddMember("uuid", rapidjson::Value(trComponent.uuid), d.GetAllocator());

    jsonComponent.AddMember(
        "type", rapidjson::Value("TransformComponent", d.GetAllocator()), d.GetAllocator());

    // position
    rapidjson::Value pos(rapidjson::kArrayType);
    pos.PushBack(trComponent.position[0], d.GetAllocator());
    pos.PushBack(trComponent.position[1], d.GetAllocator());
    pos.PushBack(trComponent.position[2], d.GetAllocator());
    jsonComponent.AddMember("position", pos, d.GetAllocator());

    // rotation
    rapidjson::Value rot(rapidjson::kArrayType);
    rot.PushBack(trComponent.rotation[0], d.GetAllocator());
    rot.PushBack(trComponent.rotation[1], d.GetAllocator());
    rot.PushBack(trComponent.rotation[2], d.GetAllocator());
    rot.PushBack(trComponent.rotation[3], d.GetAllocator());
    jsonComponent.AddMember("rotation", rot, d.GetAllocator());

    // scale
    rapidjson::Value scale(rapidjson::kArrayType);
    scale.PushBack(trComponent.scale[0], d.GetAllocator());
    scale.PushBack(trComponent.scale[1], d.GetAllocator());
    scale.PushBack(trComponent.scale[2], d.GetAllocator());
    jsonComponent.AddMember("scale", scale, d.GetAllocator());

    return jsonComponent;
}

static rapidjson::Value saveComponent(const BaseComponent& component, rapidjson::Document& d) {
    rapidjson::Value jsonComponent(rapidjson::kObjectType);

    jsonComponent.AddMember(
        "name", rapidjson::Value(component.name.c_str(), d.GetAllocator()), d.GetAllocator());
    jsonComponent.AddMember("uuid", rapidjson::Value(component.uuid), d.GetAllocator());

    jsonComponent.AddMember(
        "type", rapidjson::Value("BaseComponent", d.GetAllocator()), d.GetAllocator());

    return jsonComponent;
}

static rapidjson::Value saveComponent(
    const AudioSourceComponent& component, rapidjson::Document& d) {
    rapidjson::Value jsonComponent(rapidjson::kObjectType);

    jsonComponent.AddMember(
        "name", rapidjson::Value(component.name.c_str(), d.GetAllocator()), d.GetAllocator());
    jsonComponent.AddMember("uuid", rapidjson::Value(component.uuid), d.GetAllocator());

    jsonComponent.AddMember(
        "type", rapidjson::Value("AudioSourceComponent", d.GetAllocator()), d.GetAllocator());

    jsonComponent.AddMember("clipUuid",
        rapidjson::Value(component.clipUuid.c_str(), d.GetAllocator()), d.GetAllocator());
    jsonComponent.AddMember("loop", component.loop, d.GetAllocator());
    jsonComponent.AddMember("directional", component.directional, d.GetAllocator());

    return jsonComponent;
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

    rapidjson::Document d;
    d.SetObject();

    // save ECS and enumerate dependencies
    std::vector<int> modelIds;
    std::vector<int> textureIds;
    std::vector<int> scriptIds;
    std::vector<int> miscIds;

    // encode entities
    {
        rapidjson::Value jsonEntities(rapidjson::kArrayType);
        for (unsigned int i = 0; i < scene.entities.size(); i++) {
            rapidjson::Value jsonEntity(rapidjson::kObjectType);

            // encode entity properties
            jsonEntity.AddMember("name",
                rapidjson::Value(scene.entities[i].name.c_str(), d.GetAllocator()),
                d.GetAllocator());
            jsonEntity.AddMember(
                "uuid", rapidjson::Value(scene.entities[i].uuid), d.GetAllocator());
            jsonEntity.AddMember(
                "parent", rapidjson::Value(scene.entities[i].parent), d.GetAllocator());

            // position
            rapidjson::Value pos(rapidjson::kArrayType);
            pos.PushBack(scene.entities[i].position[0], d.GetAllocator());
            pos.PushBack(scene.entities[i].position[1], d.GetAllocator());
            pos.PushBack(scene.entities[i].position[2], d.GetAllocator());
            jsonEntity.AddMember("position", pos, d.GetAllocator());

            // rotation
            rapidjson::Value rot(rapidjson::kArrayType);
            rot.PushBack(scene.entities[i].rotation[0], d.GetAllocator());
            rot.PushBack(scene.entities[i].rotation[1], d.GetAllocator());
            rot.PushBack(scene.entities[i].rotation[2], d.GetAllocator());
            rot.PushBack(scene.entities[i].rotation[3], d.GetAllocator());
            jsonEntity.AddMember("rotation", rot, d.GetAllocator());

            // scale
            rapidjson::Value scale(rapidjson::kArrayType);
            scale.PushBack(scene.entities[i].scale[0], d.GetAllocator());
            scale.PushBack(scene.entities[i].scale[1], d.GetAllocator());
            scale.PushBack(scene.entities[i].scale[2], d.GetAllocator());
            jsonEntity.AddMember("scale", scale, d.GetAllocator());

            // encode components
            rapidjson::Value jsonComponents(rapidjson::kArrayType);

            // TransformComponent
            const std::vector<TransformComponent>& transformComponents =
                scene.entities[i].components.vecTransformComponent;
            for (unsigned int j = 0; j < transformComponents.size(); j++) {
                rapidjson::Value jsonComponent = saveComponent(transformComponents[j], d);
                jsonComponents.PushBack(jsonComponent, d.GetAllocator());
            }

            // AudioSourceComponent
            const std::vector<AudioSourceComponent>& audioSrcComponents =
                scene.entities[i].components.vecAudioSourceComponent;
            for (unsigned int j = 0; j < audioSrcComponents.size(); j++) {
                rapidjson::Value jsonComponent = saveComponent(audioSrcComponents[j], d);
                jsonComponents.PushBack(jsonComponent, d.GetAllocator());
            }

            // BaseComponent
            const std::vector<BaseComponent>& baseComponents =
                scene.entities[i].components.vecBaseComponent;
            for (unsigned int j = 0; j < baseComponents.size(); j++) {
                rapidjson::Value jsonComponent = saveComponent(baseComponents[i], d);
                jsonComponents.PushBack(jsonComponent, d.GetAllocator());
            }

            jsonEntity.AddMember("components", jsonComponents, d.GetAllocator());

            jsonEntities.PushBack(jsonEntity, d.GetAllocator());
        }
        d.AddMember("entities", jsonEntities, d.GetAllocator());
    }

    // save audio
    {
        rapidjson::Value jsonAudio(rapidjson::kArrayType);
        std::vector<audio::AudioDiskData> data;
        audio::getDiskData(data);

        for (unsigned int i = 0; i < data.size(); i++) {
            rapidjson::Value jsonAudioClip(rapidjson::kObjectType);
            jsonAudioClip.AddMember(
                "uuid", rapidjson::Value(data[i].uuid.c_str(), d.GetAllocator()), d.GetAllocator());
            jsonAudioClip.AddMember(
                "path", rapidjson::Value(data[i].path.c_str(), d.GetAllocator()), d.GetAllocator());
            jsonAudio.PushBack(jsonAudioClip, d.GetAllocator());
        }

        d.AddMember("audio", jsonAudio, d.GetAllocator());
    }

    // TODO - save models
    {
        rapidjson::Value jsonModels(rapidjson::kArrayType);
        d.AddMember("models", jsonModels, d.GetAllocator());
    }

    // TODO - save textures
    {
        rapidjson::Value jsonTextures(rapidjson::kArrayType);
        d.AddMember("textures", jsonTextures, d.GetAllocator());
    }

    // TODO - save scripts
    {
        rapidjson::Value jsonScripts(rapidjson::kArrayType);
        d.AddMember("scripts", jsonScripts, d.GetAllocator());
    }

    // TODO - save misc
    {
        rapidjson::Value jsonMisc(rapidjson::kArrayType);
        d.AddMember("misc", jsonMisc, d.GetAllocator());
    }

    // save and close document
    char writeBuf[BUFSIZ];
    rapidjson::FileWriteStream osw(file, writeBuf, BUFSIZ);
    rapidjson::Writer<rapidjson::FileWriteStream> writer(osw);
    d.Accept(writer);
    fclose(file);

    logging::logInfo("Saved level {}\n", path);
}

// save last loaded level
void saveCurrentLevel() {
    if (!currentLevelPath.empty())
        saveLevel(currentLevelPath.c_str(), *currentScene);
}

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
