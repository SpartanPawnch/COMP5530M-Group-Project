#include "animation.h"
#include <string>
#include <map>
#include <vector>
#include <memory>

#include "../logging.h"

namespace animation {
    // TODO - custom allocator
    static std::unordered_map<std::string, int> uuidToIdx;

    struct AnimationInfo {
        // redundant info for easy deletion
        std::string uuid;
        std::string path;
        std::shared_ptr<Animation> animation;
        std::weak_ptr<AnimationDescriptor> ref;

        // brace init constructor
        AnimationInfo(const std::string& _uuid, const std::string& _path,
            std::shared_ptr<Animation> _animation, std::weak_ptr<AnimationDescriptor> _ref)
            : uuid(_uuid), path(_path), animation(_animation), ref(_ref) {
        }

        // copy constructor
        AnimationInfo(AnimationInfo& aInfo) {
            // copy as normal
            uuid = aInfo.uuid;
            path = aInfo.path;
            animation = aInfo.animation;
            ref = aInfo.ref;

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;
        }

        AnimationInfo& operator=(AnimationInfo& aInfo) {
            // copy as normal
            uuid = aInfo.uuid;
            path = aInfo.path;
            animation = aInfo.animation;
            ref = aInfo.ref;

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;

            return *this;
        }

        // move constructor
        AnimationInfo(AnimationInfo&& aInfo) {
            // copy as normal
            uuid = std::move(aInfo.uuid);
            path = std::move(aInfo.path);
            animation = std::move(aInfo.animation);
            ref = std::move(aInfo.ref);

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;
        }

        // move assignment
        AnimationInfo& operator=(AnimationInfo&& aInfo) {
            // copy as normal
            uuid = std::move(aInfo.uuid);
            path = std::move(aInfo.path);
            animation = std::move(aInfo.animation);
            ref = std::move(aInfo.ref);

            // fix path reference
            if (ref.lock())
                ref.lock()->path = &path;

            return *this;
        }
    };
    static std::vector<AnimationInfo> loadedAnimations;


    // --- AudioDescriptor Methods ---
    AnimationDescriptor::AnimationDescriptor(int _idx, std::string* _path) : idx(_idx), path(_path) {
    }

    AnimationDescriptor::~AnimationDescriptor() {
        // skip if bulk clear has been performed
        if (loadedAnimations.size() == 0)
            return;

        // clear related index
        uuidToIdx.erase(loadedAnimations[idx].uuid);

        // move related audio clip to back
        for (unsigned int i = idx; i < loadedAnimations.size() - 1; i++) {
            // update pointer for all related references
            loadedAnimations[i + 1].ref.lock()->idx--;
            std::swap(loadedAnimations[i], loadedAnimations[i + 1]);
        }

        // clear audio clip
        loadedAnimations.pop_back();
    }

    // --- Module Functions ---

    std::shared_ptr<AnimationDescriptor> animationLoad(const char* path, const std::string& uuid, std::shared_ptr<model::ModelDescriptor> model) {
        // try to add new model
        if (uuidToIdx.count(uuid) == 0) {
            // construct new info element
            loadedAnimations.emplace_back(AnimationInfo{ uuid, std::string(path),
                std::make_shared<Animation>(), std::weak_ptr<AnimationDescriptor>() });

            // try to load clip
            bool loaded = loadedAnimations.back().animation->loadAnimation(path,model);

            if (loaded) {
                // loading succeeded
                uuidToIdx[uuid] = loadedAnimations.size() - 1;
                std::shared_ptr<AnimationDescriptor> ptr = std::make_shared<AnimationDescriptor>(
                    loadedAnimations.size() - 1, &loadedAnimations.back().path);
                loadedAnimations.back().ref = std::weak_ptr<AnimationDescriptor>(ptr);

                return ptr;
            }

            // loading failed
            logging::logErr("Failed to load model file {}\n", path);
            // remove failed element
            loadedAnimations.pop_back();
            // return empty descriptor
            return std::shared_ptr<AnimationDescriptor>();
        }

        // replace old clip otherwise
        int idx = uuidToIdx[uuid];
        loadedAnimations[idx].animation->loadAnimation(path,model);
        loadedAnimations[idx].path = std::string(path);
        return std::shared_ptr<AnimationDescriptor>(loadedAnimations[idx].ref);
    }

    std::shared_ptr<AnimationDescriptor> animationGetByUuid(const std::string& uuid) {
        int idx = uuidToIdx.count(uuid) ? uuidToIdx[uuid] : -1;
        if (idx >= 0)
            return std::shared_ptr<AnimationDescriptor>(loadedAnimations[idx].ref);

        return std::shared_ptr<AnimationDescriptor>();
    }

    void getDiskData(std::vector<AnimationDiskData>& data) {
        data.resize(loadedAnimations.size());
        for (unsigned int i = 0; i < loadedAnimations.size(); i++) {
            data[i].uuid = loadedAnimations[i].uuid;
            data[i].path = loadedAnimations[i].path;
        }
    }

    Bone* AnimationDescriptor::getRootBone() {
        return &loadedAnimations[idx].animation->rootBone;
    }

    float AnimationDescriptor::getTicksPerSecond() {
        return loadedAnimations[idx].animation->ticksPerSecond;
    }

    float AnimationDescriptor::getDuration() {
        return loadedAnimations[idx].animation->duration;
    }
}