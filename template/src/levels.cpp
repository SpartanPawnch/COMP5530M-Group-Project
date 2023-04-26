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
    scripting::clearScripts();
    scene.entities.clear();
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

    // TODO - load models
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

                    baseEntity.components.addComponent(cam);
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
