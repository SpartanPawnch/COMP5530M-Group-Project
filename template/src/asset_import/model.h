#pragma once

#include <memory>
#include "../model_import/model.h"
#include <map>
#include <string>
#include <vector>

namespace model {
    struct ModelDescriptor {
        int idx;
        std::string* path;
        ModelDescriptor(int _idx, std::string* _path);
        ~ModelDescriptor();
        unsigned int getVAO(int meshIndex);
        unsigned int getVBO(int meshIndex);
        unsigned int getEBO(int meshIndex);
        unsigned int getTexture(int meshIndex);
        unsigned int getIndexCount(int meshIndex);
        unsigned int getMeshCount();
        std::map<std::string, Bone> getBoneInfoMap();
        unsigned int getBoneCount();
        void setBoneCount(unsigned int bc);
        void setBoneInfoMap(std::map<std::string, Bone> bimp);
    };

    // load file with selected path. Returns non-negative index on success
    std::shared_ptr<ModelDescriptor> modelLoad(const char* path, const std::string& uuid);

    // get descriptor corresponding to uuid
    std::shared_ptr<ModelDescriptor> modelGetByUuid(const std::string& uuid);

    // get clips array
    struct ModelDiskData {
        std::string uuid;
        std::string path;
    };
    void getDiskData(std::vector<ModelDiskData>& data);
}