#pragma once

#include <memory>
#include <string>
#include <vector>
#include "../model_import/animation.h"
#include "../model_import/bone.h"
#include "model.h"

struct AssimpNodeData;

namespace animation {
    struct AnimationDescriptor {
        int idx;
        std::string* path;
        AnimationDescriptor(int _idx, std::string* _path);
        ~AnimationDescriptor();
        AssimpNodeData* getRootNode();
        Bone* findBoneByName(const std::string& name);
        float getFPS();
        float getDuration();
        std::map<std::string, BoneInfo> getBoneInfoMap();
    };

    // load file with selected path. Returns non-negative index on success
    std::shared_ptr<AnimationDescriptor> animationLoad(const char* path, const std::string& uuid, std::shared_ptr<model::ModelDescriptor> model);

    // get descriptor corresponding to uuid
    std::shared_ptr<AnimationDescriptor> animationGetByUuid(const std::string& uuid);

    // get clips array
    struct AnimationDiskData {
        std::string uuid;
        std::string path;
    };
    void getDiskData(std::vector<AnimationDiskData>& data);
}