//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "skeletalmodel.h"
#include "bone.h"

struct AssimpNodeData
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<AssimpNodeData> children;
};

class Animation
{
public:
    float duration;
    int framesPerSecond;
    std::vector<Bone> bones;
    AssimpNodeData rootNode;
    std::map<std::string, BoneInfo> boneInfoMap;

    Animation() = default;
    Animation(const std::string& animationPath, SkeletalModel* model);
    ~Animation();

    Bone* findBoneByName(const std::string& name);
    void readMissingBones(const aiAnimation* animation, SkeletalModel& model);
    void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
};