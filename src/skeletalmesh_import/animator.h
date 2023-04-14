//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "animation.h"

struct NoConditionACTransition;
struct BoolACTransition;
struct IntACTransition;
struct FloatACTransition;

struct AnimationControllerNode {
	std::string name;
	Animation* animation;
	unsigned int loopCount; //0 = infinite loop, 1 = play once, 2 = play twice, etc...
	std::vector<NoConditionACTransition> noConditionTransitions;
	std::vector<BoolACTransition> boolTransitions;
	std::vector<IntACTransition> intTransitions;
	std::vector<FloatACTransition> floatTransitions;
};

struct NoConditionACTransition {
	AnimationControllerNode* transitionTo;
};
struct BoolACTransition {
	AnimationControllerNode* transitionTo;
	bool immediate; //should translate instantly or only when animation is done
	bool condition;
	bool desiredValue;
};
struct IntACTransition {
	AnimationControllerNode* transitionTo;
	bool immediate;
	int condition;
	int desiredValue;
	bool shouldBeGreater;
	bool shouldBeEqual;
	bool shouldBeLower;
};
struct FloatACTransition {
	AnimationControllerNode* transitionTo;
	bool immediate;
	float condition;
	float desiredValue;
	bool shouldBeGreater;
	bool shouldBeEqual;
	bool shouldBeLower;
};

class Animator
{
public:
	std::vector<Animation> animations;
	std::vector<AnimationControllerNode> nodes;
	AnimationControllerNode* currentNode;
	AnimationControllerNode* nextNode;
	AnimationControllerNode* selectedNode;
	float currentTime;
	int currentLoopCount;
	float deltaTime;
	std::vector <glm::mat4> transformMatrices;

	Animator();
	void onStart();
	void onUpdate(float df);
	void onLoop();
	void transitionToNode(AnimationControllerNode* node);
	void loadAnimation(const std::string& animationPath, SkeletalModel* model);
	void onEnd();

	void addNode();
	void addNoConditionTransition();
	void addBoolACTransition();
	void addIntACTransition();
	void addFloatACTransition();
	
	void updateAnimation(float dt);
	void playAnimation(Animation* Animation);
	void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);
};