//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "animation.h"

class Animator
{
public:
	std::vector <glm::mat4> transformMatrices;
	Animation* currentAnimation;
	float currentTime;
	float deltaTime;

	Animator(Animation* Animation);
	void updateAnimation(float dt);
	void playAnimation(Animation* Animation);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
};