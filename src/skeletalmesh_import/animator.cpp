#include "animator.h"

#define MAX_BONES 100

Animator::Animator() {
	currentTime = 0.0;
	currentLoopCount = 0;
	currentNode = nullptr;
	selectedNode = nullptr;

	transformMatrices.reserve(MAX_BONES);

	for (unsigned int i = 0; i < MAX_BONES; i++) {
		transformMatrices.push_back(glm::mat4(1.0f));
	}
}

void Animator::updateAnimation(float dt) {
	deltaTime = dt;
	Animation* currentAnimation = currentNode->animation;
	if (currentAnimation) {
		currentTime += currentAnimation->framesPerSecond * dt;
		currentTime = fmod(currentTime, currentAnimation->duration);
		calculateBoneTransform(&currentAnimation->rootNode, glm::mat4(1.0f));
	}
}

void Animator::playAnimation(Animation* Animation) {
	//currentAnimation = Animation;
	//currentTime = 0.0f;
}

void Animator::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
	Animation* currentAnimation = currentNode->animation;

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

void Animator::onStart() {

}

void Animator::onUpdate(float df) {
	if (!(currentNode)) {
		if (nodes.size() > 0) {
			currentNode = &nodes[0];
		}
		else {
			return;
		}
	}

	updateAnimation(df);

	for each (NoConditionACTransition transition in currentNode->noConditionTransitions)
	{
		if (currentNode->loopCount > 0 && currentLoopCount > currentNode->loopCount) {
			transitionToNode(transition.transitionTo);
			return;
		}
	}

	for each (BoolACTransition transition in currentNode->boolTransitions)
	{
		if (transition.condition == transition.desiredValue) {
			if (transition.immediate) {
				transitionToNode(transition.transitionTo);
				return;
			}
			else if(!(nextNode)){
				nextNode = transition.transitionTo;
			}
		}
	}

	for each (IntACTransition transition in currentNode->intTransitions)
	{
		if ((transition.shouldBeLower && transition.condition < transition.desiredValue) || (transition.shouldBeEqual && transition.condition == transition.desiredValue) || (transition.shouldBeGreater && transition.condition > transition.desiredValue)) {
			if (transition.immediate) {
				transitionToNode(transition.transitionTo);
				return;
			}
			else if (!(nextNode)) {
				nextNode = transition.transitionTo;
			}
		}
	}

	for each (FloatACTransition transition in currentNode->floatTransitions)
	{
		if ((transition.shouldBeLower && transition.condition < transition.desiredValue) || (transition.shouldBeEqual && transition.condition == transition.desiredValue) || (transition.shouldBeGreater && transition.condition > transition.desiredValue)) {
			if (transition.immediate) {
				transitionToNode(transition.transitionTo);
				return;
			}
			else if (!(nextNode)) {
				nextNode = transition.transitionTo;
			}
		}
	}
}

void Animator::onLoop() {
	currentLoopCount++;
	if (nextNode) {
		transitionToNode(nextNode);
	}
}

void Animator::transitionToNode(AnimationControllerNode* node) {
	currentNode = node;
	currentTime = 0;
	currentLoopCount = 0;
	nextNode = nullptr;
}

void Animator::loadAnimation(const std::string& animationPath, SkeletalModel* model) {
	animations.push_back(Animation(animationPath, model));
}

void Animator::onEnd() {

}


void Animator::addNode() {
	AnimationControllerNode node;
	node.name = "New Node";
	node.animation = &animations[0];
	node.loopCount=0;
	nodes.push_back(node);
	selectedNode = &nodes[nodes.size() - 1];
}

void Animator::addNoConditionTransition() {
	if (!(selectedNode)) {
		return;
	}
	NoConditionACTransition transition;
	transition.transitionTo = selectedNode;
	selectedNode->noConditionTransitions.push_back(transition);
}

void Animator::addBoolACTransition() {
	if (!(selectedNode)) {
		return;
	}
	BoolACTransition transition;
	transition.transitionTo = selectedNode;
	transition.immediate = false;
	transition.condition = false;
	transition.desiredValue = true;
	selectedNode->boolTransitions.push_back(transition);
}

void Animator::addIntACTransition() {
	if (!(selectedNode)) {
		return;
	}
	IntACTransition transition;
	transition.transitionTo = selectedNode;
	transition.immediate = false;
	transition.condition = 1;
	transition.desiredValue = 10;
	transition.shouldBeLower = false;
	transition.shouldBeEqual = true;
	transition.shouldBeGreater = false;
	selectedNode->intTransitions.push_back(transition);
}

void Animator::addFloatACTransition() {
	if (!(selectedNode)) {
		return;
	}
	FloatACTransition transition;
	transition.transitionTo = selectedNode;
	transition.immediate = false;
	transition.condition = 1.0;
	transition.desiredValue = 10.0;
	transition.shouldBeLower = false;
	transition.shouldBeEqual = false;
	transition.shouldBeGreater = true;
	selectedNode->floatTransitions.push_back(transition);
}

