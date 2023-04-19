#pragma once

#include <memory>
#include <string>

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
}