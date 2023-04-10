#include "animator.h"

#define MAX_BONES 100

Animator::Animator(Animation* Animation) {
	currentTime = 0.0;
	currentAnimation = Animation;

	//TODO: add animation controller functionality
	transformMatrices.reserve(MAX_BONES);

	for (unsigned int i = 0; i < MAX_BONES; i++) {
		transformMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::updateAnimation(float dt) {
	deltaTime = dt;
	if (currentAnimation) {
		currentTime += currentAnimation->framesPerSecond * dt;
		currentTime = fmod(currentTime, currentAnimation->duration);
		calculateBoneTransform(&currentAnimation->rootNode, glm::mat4(1.0f));
	}
}

void Animator::playAnimation(Animation* Animation) {
	currentAnimation = Animation;
	currentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = currentAnimation->findBoneByName(nodeName);

	if (Bone)
	{
		Bone->update(currentTime);
		nodeTransform = Bone->transform;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = currentAnimation->boneInfoMap;
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		transformMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		calculateBoneTransform(&node->children[i], globalTransformation);
}
