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
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "ECS/Scene/Scene.h"
#include "ECS/Entity/CameraEntity.h"
#include "ECS/Entity/ModelEntity.h"
#include "ECS/Entity/SkeletalMeshEntity.h"
#include "ECS/Component/BaseComponent.h"
#include "ECS/Component/TransformComponent.h"

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
    audio::audioClearAll();
    clearDynamicTextures();
    // TODO move cleanup to entity destructor
    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        for (unsigned int j = 0; j < scene.entities[i].components.size(); j++) {
            if (scene.entities[i].components[j])
                delete scene.entities[i].components[j];
        }
        scene.entities[i].components.clear();
    }
    scene.entities.clear();
    scene.selectedEntity = nullptr;

    // load audio
    {
        assert(doc.HasMember("audio"));
        auto jsonAudio = doc["audio"].GetArray();
        for (unsigned int i = 0; i < jsonAudio.Size(); i++) {
            // TODO init with fixed uuid
            // audio::audioLoad(jsonAudio[i]["path"].GetString());
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
            BaseEntity jsonEntity;

            jsonEntity.uuid = entity["uuid"].GetInt();
            jsonEntity.name = std::string(entity["name"].GetString());

            // add components
            auto jsonComponents = entity["components"].GetArray();
            for (unsigned int j = 0; j < jsonComponents.Size(); j++) {
                auto jsonComponent = jsonComponents[j].GetObject();

                // TransformComponent
                if (strcmp(jsonComponent["type"].GetString(), "TransformComponent") == 0) {
                    TransformComponent* trComponent = new TransformComponent();
                    trComponent->position = glm::vec3(jsonComponent["position"][0].GetFloat(),
                        jsonComponent["position"][1].GetFloat(),
                        jsonComponent["position"][2].GetFloat());
                    trComponent->rotation = glm::vec4(jsonComponent["rotation"][0].GetFloat(),
                        jsonComponent["rotation"][1].GetFloat(),
                        jsonComponent["rotation"][2].GetFloat(),
                        jsonComponent["rotation"][3].GetFloat());
                    trComponent->scale = glm::vec3(jsonComponent["scale"][0].GetFloat(),
                        jsonComponent["scale"][1].GetFloat(), jsonComponent["scale"][2].GetFloat());
                    jsonEntity.components.emplace_back(trComponent);
                }
                // BaseComponent
                else {
                    jsonEntity.components.emplace_back(new BaseComponent());
                }
                jsonEntity.components.back()->name = std::string(jsonComponent["name"].GetString());
                jsonEntity.components.back()->uuid = jsonComponent["uuid"].GetInt();
            }
            scene.entities.emplace_back(jsonEntity);
        }
    }

    // set level to current if everything is succesful
    currentLevelPath = path;
    currentScene = &scene;
    logging::logInfo("Opened level {}\n", path);
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
    std::vector<int> audioIds;
    std::vector<int> modelIds;
    std::vector<int> textureIds;
    std::vector<int> scriptIds;
    std::vector<int> miscIds;

    // encode entities
    {
        rapidjson::Value jsonEntities(rapidjson::kArrayType);
        for (unsigned int i = 0; i < scene.entities.size(); i++) {
            rapidjson::Value jsonEntity(rapidjson::kObjectType);

            jsonEntity.AddMember("name",
                rapidjson::Value(scene.entities[i].name.c_str(), d.GetAllocator()),
                d.GetAllocator());
            jsonEntity.AddMember(
                "uuid", rapidjson::Value(scene.entities[i].uuid), d.GetAllocator());

            // encode components
            rapidjson::Value jsonComponents(rapidjson::kArrayType);
            for (unsigned int j = 0; j < scene.entities[i].components.size(); j++) {
                rapidjson::Value jsonComponent(rapidjson::kObjectType);

                jsonComponent.AddMember("name",
                    rapidjson::Value(
                        scene.entities[i].components[j]->name.c_str(), d.GetAllocator()),
                    d.GetAllocator());
                jsonComponent.AddMember("uuid",
                    rapidjson::Value(scene.entities[i].components[j]->uuid), d.GetAllocator());

                // TransformComponent
                if (auto trComponent =
                        dynamic_cast<TransformComponent*>(scene.entities[i].components[j])) {
                    jsonComponent.AddMember("type",
                        rapidjson::Value("TransformComponent", d.GetAllocator()), d.GetAllocator());

                    // position
                    rapidjson::Value pos(rapidjson::kArrayType);
                    pos.PushBack(trComponent->position[0], d.GetAllocator());
                    pos.PushBack(trComponent->position[1], d.GetAllocator());
                    pos.PushBack(trComponent->position[2], d.GetAllocator());
                    jsonComponent.AddMember("position", pos, d.GetAllocator());

                    // rotation
                    rapidjson::Value rot(rapidjson::kArrayType);
                    rot.PushBack(trComponent->rotation[0], d.GetAllocator());
                    rot.PushBack(trComponent->rotation[1], d.GetAllocator());
                    rot.PushBack(trComponent->rotation[2], d.GetAllocator());
                    rot.PushBack(trComponent->rotation[3], d.GetAllocator());
                    jsonComponent.AddMember("rotation", rot, d.GetAllocator());

                    // scale
                    rapidjson::Value scale(rapidjson::kArrayType);
                    scale.PushBack(trComponent->scale[0], d.GetAllocator());
                    scale.PushBack(trComponent->scale[1], d.GetAllocator());
                    scale.PushBack(trComponent->scale[2], d.GetAllocator());
                    jsonComponent.AddMember("scale", scale, d.GetAllocator());
                }
                // BaseComponent
                else {
                    jsonComponent.AddMember("type",
                        rapidjson::Value("BaseComponent", d.GetAllocator()), d.GetAllocator());
                }

                jsonComponents.PushBack(jsonComponent, d.GetAllocator());
            }
            jsonEntity.AddMember("components", jsonComponents, d.GetAllocator());

            jsonEntities.PushBack(jsonEntity, d.GetAllocator());
        }
        d.AddMember("entities", jsonEntities, d.GetAllocator());
    }

    // TODO - save audio
    {
        rapidjson::Value jsonAudio(rapidjson::kArrayType);
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
