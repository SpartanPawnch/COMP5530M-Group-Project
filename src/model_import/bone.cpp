#include "bone.h"

Bone::Bone(const std::string& n, int i, const aiNodeAnim* c) {
	name = n;
	id = i;
	transform = glm::mat4(1.0);

	positionKeys.resize(c->mNumPositionKeys);
	rotationKeys.resize(c->mNumRotationKeys);
	scaleKeys.resize(c->mNumScalingKeys);

	positionKeysCount = c->mNumPositionKeys;
	rotationKeysCount = c->mNumRotationKeys;
	scaleKeysCount = c->mNumScalingKeys;

	for (unsigned int i = 0; i < positionKeysCount; i++) {
		aiVector3D aiPosition = c->mPositionKeys[i].mValue;
		float time = c->mPositionKeys[i].mTime;

		KeyPosition currentKey;
		currentKey.position = glm::vec3(aiPosition.x, aiPosition.y, aiPosition.z);
		currentKey.time = time;

		positionKeys.push_back(currentKey);
	}

	for (unsigned int i = 0; i < rotationKeysCount; i++) {
		aiQuaternion aiRotation = c->mRotationKeys[i].mValue;
		float time = c->mRotationKeys[i].mTime;

		KeyRotation currentKey;
		currentKey.rotation = glm::quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);
		currentKey.time = time;

		rotationKeys.push_back(currentKey);
	}

	for (unsigned int i = 0; i < scaleKeysCount; i++) {
		aiVector3D aiScale = c->mScalingKeys[i].mValue;
		float time = c->mScalingKeys[i].mTime;

		KeyScale currentKey;
		currentKey.scale = glm::vec3(aiScale.x, aiScale.y, aiScale.z);
		currentKey.time = time;

		scaleKeys.push_back(currentKey);
	}
}

void Bone::update(float animationTime) {
	glm::mat4 translation = interpolatePosition(animationTime);
	glm::mat4 rotation = interpolateRotation(animationTime);
	glm::mat4 scale = interpolateScale(animationTime);
	transform = translation * rotation * scale;
}

int Bone::getPositionIndex(float animationTime) {
	for (unsigned int i = 0; i < positionKeysCount - 1; i++) {
		if (animationTime < positionKeys.at(i + 1).time) {
			return i;
		}
	}
	return 0;
	//assert(0);
}

int Bone::getRotationIndex(float animationTime) {
	for (unsigned int i = 0; i < rotationKeysCount - 1; i++) {
		if (animationTime < rotationKeys.at(i + 1).time) {
			return i;
		}
	}
	return 0;
	//assert(0);
}

int Bone::getScaleIndex(float animationTime) {
	for (unsigned int i = 0; i < scaleKeysCount - 1; i++) {
		if (animationTime < scaleKeys.at(i + 1).time) {
			return i;
		}
	}
	return 0;
	//assert(0);
}

float Bone::getInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
	float interpolationFactor = 0.0f;
	float timeBetweenFrames = nextTimeStamp - lastTimeStamp;
	float timeMissingForNext = animationTime - lastTimeStamp;
	return timeMissingForNext / timeBetweenFrames;
}

glm::mat4 Bone::interpolatePosition(float animationTime) {
	if (positionKeys.size() == 1) {
		return glm::translate(glm::mat4(1.0f), positionKeys.at(0).position);
	}
	//TODO:maybe check if there are no keyframes and check if the prev is the last one
	int prevKeyId = getPositionIndex(animationTime);
	int nextKeyId = prevKeyId + 1;
	float interpolationFactor = getInterpolationFactor(positionKeys.at(prevKeyId).time, positionKeys.at(nextKeyId).time, animationTime);
	glm::vec3 keyPosition = glm::mix(positionKeys.at(prevKeyId).position, positionKeys.at(nextKeyId).position, interpolationFactor);
	return glm::translate(glm::mat4(1.0f), keyPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime) {
	if (rotationKeys.size() == 1) {
		auto rotation = glm::normalize(rotationKeys.at(0).rotation);
		return glm::toMat4(rotation);
	}

	int prevKeyId = getRotationIndex(animationTime);
	int nextKeyId = prevKeyId + 1;
	float interpolationFactor = getInterpolationFactor(rotationKeys.at(prevKeyId).time, rotationKeys.at(nextKeyId).time, animationTime);
	glm::quat keyRotation = glm::slerp(rotationKeys.at(prevKeyId).rotation, rotationKeys.at(nextKeyId).rotation, animationTime);
	keyRotation = glm::normalize(keyRotation);
	return glm::toMat4(keyRotation);
}

glm::mat4 Bone::interpolateScale(float animationTime) {
	if (scaleKeys.size() == 1) {
		return glm::scale(glm::mat4(1.0f), scaleKeys.at(0).scale);
	}

	int prevKeyId = getScaleIndex(animationTime);
	int nextKeyId = prevKeyId + 1;
	float interpolationFactor = getInterpolationFactor(scaleKeys.at(prevKeyId).time, scaleKeys.at(nextKeyId).time, animationTime);
	glm::vec3 keyScale = glm::mix(scaleKeys.at(prevKeyId).scale, scaleKeys.at(nextKeyId).scale, interpolationFactor);
	return glm::scale(glm::mat4(1.0f), keyScale);
}