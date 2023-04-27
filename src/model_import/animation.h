//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "../asset_import/model.h"
#include "../asset_import/animation.h"
#include "model.h"
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
    std::string name;
    float duration;
    int framesPerSecond;
    std::vector<Bone> bones;
    AssimpNodeData rootNode;
    std::map<std::string, BoneInfo> boneInfoMap;

    Animation();
    ~Animation();

    bool loadAnimation(const std::string& animationPath, std::shared_ptr<model::ModelDescriptor> model);
    Bone* findBoneByName(const std::string& name);
    void readMissingBones(const aiAnimation* animation, std::shared_ptr<model::ModelDescriptor>& model);
    void readHierarchyData(AssimpNodeData& dest, const aiNode* src);
};