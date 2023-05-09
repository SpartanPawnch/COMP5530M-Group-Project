#include "animation.h"

Animation::Animation() {
    name = "New Animation";
    duration = 0.0f;
    ticksPerSecond = 0.0f;
}

Animation::~Animation() {
}

bool Animation::loadAnimation(const std::string& animationPath,
    std::shared_ptr<model::ModelDescriptor> model) {
    Assimp::Importer importer;
    uint32_t flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights;
    const aiScene* scene = importer.ReadFile(animationPath, flags);
    if (!scene->HasAnimations()) {
        return false;
    }
    auto animation = scene->mAnimations[0];
    name = animation->mName.data;
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;

    std::map<std::string, Bone>& modelBoneInfoMap = model->getBoneInfoMap();

    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* channel = animation->mChannels[i];
        std::string bone_name(channel->mNodeName.C_Str());
        Bone bone = modelBoneInfoMap[bone_name];
        boneInfoMap[bone_name] = bone;
    }

    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        aiNodeAnim* channel = animation->mChannels[i];
        std::string bone_name(channel->mNodeName.C_Str());

        if (boneInfoMap.count(bone_name)) {
            auto& bone_data = boneInfoMap.at(bone_name);

            for (unsigned int j = 0; j < channel->mNumPositionKeys; j++) {
                aiVector3D aiPosition = channel->mPositionKeys[j].mValue;
                float time = channel->mPositionKeys[j].mTime;

                KeyPosition currentKey;
                currentKey.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
                //std::cout << "Position: " << glm::to_string(currentKey.position) << std::endl;
                currentKey.time = time;
                bone_data.positionKeys.push_back(currentKey);
            }

            for (uint32_t j = 0; j < channel->mNumRotationKeys; j++) {
                aiQuaternion aiRotation = channel->mRotationKeys[j].mValue;
                float time = channel->mRotationKeys[j].mTime;

                KeyRotation currentKey;
                currentKey.rotation = glm::quat(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
                //std::cout << "Rotation: " << glm::to_string(currentKey.rotation) << std::endl;
                currentKey.time = time;
                bone_data.rotationKeys.push_back(currentKey);
            }

            for (uint32_t j = 0; j < channel->mNumScalingKeys; j++) {
                aiVector3D aiScale = channel->mScalingKeys[j].mValue;
                float time = channel->mScalingKeys[j].mTime;

                KeyScale currentKey;
                currentKey.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
                //std::cout << "Scale: " << glm::to_string(currentKey.scale) << std::endl;
                currentKey.time = time;
                bone_data.scaleKeys.push_back(currentKey);
            }
        }
    }

    setBoneData(scene->mRootNode, rootBone);

    return true;
}

void Animation::setBoneData(aiNode* node, Bone& bone) {
    std::string bone_name(node->mName.C_Str());

    if (boneInfoMap.count(bone_name)) {
        bone = boneInfoMap.at(bone_name);
        //std::cout << bone.id << std::endl;

        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            Bone child;
            setBoneData(node->mChildren[i], child);
            bone.children.push_back(child);
        }
    }
    else {
        for (uint32_t i = 0; i < node->mNumChildren; i++) {
            setBoneData(node->mChildren[i], bone);
        }
    }
}
