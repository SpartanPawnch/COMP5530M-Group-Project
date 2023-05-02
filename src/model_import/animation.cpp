#include "animation.h"

Animation::Animation() {
    name = "New Animation";
    duration = 0.0f;
    ticksPerSecond = 0.0f;
}

Animation::~Animation() {

}

bool Animation::loadAnimation(const std::string& animationPath, std::shared_ptr<model::ModelDescriptor> model) {
    Assimp::Importer importer;
	uint32_t flags = aiProcess_Triangulate | aiProcess_FlipUVs;
    const aiScene* scene = importer.ReadFile(animationPath, flags);
    if (!scene->HasAnimations()) {
        return false;
    }
    auto animation = scene->mAnimations[0];
    name = animation->mName.data;
    duration = animation->mDuration;
    ticksPerSecond = animation->mTicksPerSecond;
	std::map<std::string, Bone>& boneInfoMap = model->getBoneInfoMap();

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
				std::cout << glm::to_string(currentKey.position) << std::endl;
				currentKey.time = time;
				bone_data.positionKeys.push_back(currentKey);
			}

			for (uint32_t j = 0; j < channel->mNumRotationKeys; j++) {
				aiQuaternion aiRotation = channel->mRotationKeys[j].mValue;
				float time = channel->mRotationKeys[j].mTime;

				KeyRotation currentKey;
				currentKey.rotation = glm::quat(aiRotation.w, aiRotation.x, aiRotation.y, aiRotation.z);
				currentKey.time = time;
				bone_data.rotationKeys.push_back(currentKey);
			}

			for (uint32_t j = 0; j < channel->mNumScalingKeys; j++) {
				aiVector3D aiScale = channel->mPositionKeys[j].mValue;
				float time = channel->mPositionKeys[j].mTime;

				KeyScale currentKey;
				currentKey.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
				currentKey.time = time;
				bone_data.scaleKeys.push_back(currentKey);
			}
		}
	}

	setBoneData(scene->mRootNode, rootBone, boneInfoMap);

    return true;
}

void Animation::setBoneData(aiNode* node, Bone& bone, std::map<std::string, Bone>& boneInfoMap) {
	std::string bone_name(node->mName.C_Str());

	if (boneInfoMap.count(bone_name)) {
		bone = boneInfoMap.at(bone_name);

		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			Bone child;
			setBoneData(node->mChildren[i], child, boneInfoMap);
			bone.children.push_back(child);
		}
	}
	else {
		for (uint32_t i = 0; i < node->mNumChildren; i++) {
			setBoneData(node->mChildren[i], bone, boneInfoMap);
		}
	}
}