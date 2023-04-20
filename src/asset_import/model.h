#pragma once

#include <memory>
#include <string>
#include <vector>

namespace model {
    struct ModelDescriptor {
        int idx;
        std::string* path;
        ModelDescriptor(int _idx, std::string* _path);
        ~ModelDescriptor();
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