#include "model.h"
#include <string>
#include <vector>
#include <memory>

#include "../logging.h"

namespace model {
    // TODO - custom allocator
    static std::unordered_map<std::string, int> uuidToIdx;

    struct ModelInfo {
        // redundant info for easy deletion
        std::string uuid;
        std::string path;
        std::shared_ptr<Model> model;
        std::weak_ptr<ModelDescriptor> ref;

        // brace init constructor
        ModelInfo(const std::string& _uuid, const std::string& _path, std::shared_ptr<Model> _model,
            std::weak_ptr<ModelDescriptor> _ref)
            : uuid(_uuid), path(_path), model(_model), ref(_ref) {
        }

        // copy constructor
        ModelInfo(ModelInfo& aInfo) {
            // copy as normal
            uuid = aInfo.uuid;
            path = aInfo.path;
            model = aInfo.model;
            ref = aInfo.ref;

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;
        }

        ModelInfo& operator=(ModelInfo& aInfo) {
            // copy as normal
            uuid = aInfo.uuid;
            path = aInfo.path;
            model = aInfo.model;
            ref = aInfo.ref;

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;

            return *this;
        }

        // move constructor
        ModelInfo(ModelInfo&& aInfo) {
            // copy as normal
            uuid = std::move(aInfo.uuid);
            path = std::move(aInfo.path);
            model = std::move(aInfo.model);
            ref = std::move(aInfo.ref);

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;
        }

        // move assignment
        ModelInfo& operator=(ModelInfo&& aInfo) {
            // copy as normal
            uuid = std::move(aInfo.uuid);
            path = std::move(aInfo.path);
            model = std::move(aInfo.model);
            ref = std::move(aInfo.ref);

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;

            return *this;
        }
    };
    static std::vector<ModelInfo> loadedModels;

    // --- AudioDescriptor Methods ---
    ModelDescriptor::ModelDescriptor(int _idx, std::string* _path) : idx(_idx), path(_path) {
    }

    ModelDescriptor::~ModelDescriptor() {
        // skip if bulk clear has been performed
        if (loadedModels.size() == 0)
            return;

        loadedModels[idx].model->deleteBuffers();

        // clear related index
        uuidToIdx.erase(loadedModels[idx].uuid);

        // move related audio clip to back
        for (unsigned int i = idx; i < loadedModels.size() - 1; i++) {
            // update pointer for all related references
            loadedModels[i + 1].ref.lock()->idx--;
            std::swap(loadedModels[i], loadedModels[i + 1]);
        }

        // clear audio clip
        loadedModels.pop_back();
    }

    // --- Module Functions ---

    std::shared_ptr<ModelDescriptor> modelLoad(const char* path, const std::string& uuid) {
        // try to add new model
        if (uuidToIdx.count(uuid) == 0) {
            // construct new info element
            loadedModels.emplace_back(ModelInfo{uuid, std::string(path), std::make_shared<Model>(),
                std::weak_ptr<ModelDescriptor>()});

            // try to load clip
            bool loaded = loadedModels.back().model->loadModel(path);

            if (loaded) {
                // loading succeeded
                uuidToIdx[uuid] = loadedModels.size() - 1;
                std::shared_ptr<ModelDescriptor> ptr = std::make_shared<ModelDescriptor>(
                    loadedModels.size() - 1, &loadedModels.back().path);
                loadedModels.back().ref = std::weak_ptr<ModelDescriptor>(ptr);

                return ptr;
            }

            // loading failed
            logging::logErr("Failed to load model file {}\n", path);
            // remove failed element
            loadedModels.pop_back();
            // return empty descriptor
            return std::shared_ptr<ModelDescriptor>();
        }

        // replace old clip otherwise
        int idx = uuidToIdx[uuid];
        loadedModels[idx].model->loadModel(path);
        loadedModels[idx].path = std::string(path);
        return std::shared_ptr<ModelDescriptor>(loadedModels[idx].ref);
    }

    std::shared_ptr<ModelDescriptor> modelGetByUuid(const std::string& uuid) {
        int idx = uuidToIdx.count(uuid) ? uuidToIdx[uuid] : -1;
        if (idx >= 0)
            return std::shared_ptr<ModelDescriptor>(loadedModels[idx].ref);

        return std::shared_ptr<ModelDescriptor>();
    }

    void getDiskData(std::vector<ModelDiskData>& data) {
        data.resize(loadedModels.size());
        for (unsigned int i = 0; i < loadedModels.size(); i++) {
            data[i].uuid = loadedModels[i].uuid;
            data[i].path = loadedModels[i].path;
        }
    }

    unsigned int ModelDescriptor::getVAO(int meshIndex) {
        return loadedModels[idx].model->meshes[meshIndex].VAO;
    }

    unsigned int ModelDescriptor::getVBO(int meshIndex) {
        return loadedModels[idx].model->meshes[meshIndex].VBO;
    }

    unsigned int ModelDescriptor::getEBO(int meshIndex) {
        return loadedModels[idx].model->meshes[meshIndex].EBO;
    }

    //unsigned int ModelDescriptor::getTexture(int meshIndex) {
    //    if (loadedModels[idx].model->textures_loaded.size() <= meshIndex ||
    //        loadedModels[idx].model->textures_loaded[meshIndex].textureDescriptor == nullptr) {
    //        return 0;
    //    }
    //    return loadedModels[idx].model->textures_loaded[meshIndex].textureDescriptor->texId;
    //}

    unsigned int ModelDescriptor::getIndexCount(int meshIndex) {
        return loadedModels[idx].model->meshes[meshIndex].indices.size();
    }

    unsigned int ModelDescriptor::getMeshCount() {
        return loadedModels[idx].model->meshes.size();
    }

    
    std::string ModelDescriptor::getMeshName(int meshIndex) {
        return loadedModels[idx].model->meshes[meshIndex].name;
    }

    std::string ModelDescriptor::getMeshMaterialName(int meshIndex) {
        return loadedModels[idx].model->meshes[meshIndex].material->name;
    }

    bool ModelDescriptor::meshHasMaterial(int meshIndex) {
        return !(loadedModels[idx].model->meshes[meshIndex].material == nullptr);
    }

    void ModelDescriptor::setMeshMaterial(int meshIndex, std::shared_ptr<ActiveMaterial> mat) {
        loadedModels[idx].model->meshes[meshIndex].material = mat;
    }


    std::map<std::string, Bone> ModelDescriptor::getBoneInfoMap() {
        return loadedModels[idx].model->boneInfoMap;
    }

    void ModelDescriptor::setBoneInfoMap(std::map<std::string, Bone> bimp) {
        loadedModels[idx].model->boneInfoMap = bimp;
    }

    unsigned int ModelDescriptor::getBoneCount() {
        return loadedModels[idx].model->boneCounter;
    }

    void ModelDescriptor::setBoneCount(unsigned int bc) {
        loadedModels[idx].model->boneCounter = bc;
    }
}
