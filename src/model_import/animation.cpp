#include "animation.h"

Animation::Animation() {
    name = "New Animation";
    duration = 0.0f;
    framesPerSecond = 0.0f;
}

Animation::~Animation() {
}

bool Animation::loadAnimation(const std::string& animationPath,
    std::shared_ptr<model::ModelDescriptor> model) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(animationPath, aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    name = "New Animation";
    duration = animation->mDuration;
    framesPerSecond = animation->mTicksPerSecond;
    readHierarchyData(rootNode, scene->mRootNode);
    readMissingBones(animation, model);
    return true;
}

Bone* Animation::findBoneByName(const std::string& name) {
    auto iter = std::find_if(bones.begin(), bones.end(),
        [&](const Bone& Bone) { return Bone.name == name; });
    if (iter == bones.end())
        return nullptr;
    else
        return &(*iter);
}

void Animation::readMissingBones(const aiAnimation* animation,
    std::shared_ptr<model::ModelDescriptor>& model) {
    int channelCount = animation->mNumChannels;

    std::map<std::string, BoneInfo> bInfoMap = model->getBoneInfoMap();
    unsigned int boneCount = model->getBoneCount();

    for (unsigned int i = 0; i < channelCount; i++) {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;
        if (bInfoMap.find(boneName) == bInfoMap.end()) {
            bInfoMap[boneName].id = boneCount;
            boneCount++;
            model->setBoneCount(boneCount);
        }
        bones.push_back(
            Bone(channel->mNodeName.data, bInfoMap[channel->mNodeName.data].id, channel));
    }
    boneInfoMap = bInfoMap;
    model->setBoneInfoMap(bInfoMap);
}

void Animation::readHierarchyData(AssimpNodeData& dest, const aiNode* src) {
    assert(src);

    dest.name = src->mName.data;
    dest.transformation[0][0] = src->mTransformation.a1;
    dest.transformation[1][0] = src->mTransformation.a2;
    dest.transformation[2][0] = src->mTransformation.a3;
    dest.transformation[3][0] = src->mTransformation.a4;
    dest.transformation[0][1] = src->mTransformation.b1;
    dest.transformation[1][1] = src->mTransformation.b2;
    dest.transformation[2][1] = src->mTransformation.b3;
    dest.transformation[3][1] = src->mTransformation.b4;
    dest.transformation[0][2] = src->mTransformation.c1;
    dest.transformation[1][2] = src->mTransformation.c2;
    dest.transformation[2][2] = src->mTransformation.c3;
    dest.transformation[2][2] = src->mTransformation.c4;
    dest.transformation[0][3] = src->mTransformation.d1;
    dest.transformation[1][3] = src->mTransformation.d2;
    dest.transformation[2][3] = src->mTransformation.d3;
    dest.transformation[3][3] = src->mTransformation.d4;
    dest.childrenCount = src->mNumChildren;

    for (unsigned int i = 0; i < src->mNumChildren; i++) {
        AssimpNodeData newData;
        readHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}
