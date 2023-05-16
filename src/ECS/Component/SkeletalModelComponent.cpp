#include "SkeletalModelComponent.h"

#include "../../model_import/model.h"

void SkeletalModelComponent::start() {
}
void SkeletalModelComponent::update(float dt) {
	if (!isPlaying) return;

	if (!(currentNode)) {
		if (nodes.size() > 0) {
			currentNode = &nodes[0];
		}
		else {
			return;
		}
	}

	updateAnimation(dt);

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
			else if (!(nextNode)) {
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
void SkeletalModelComponent::stop() {
}

void SkeletalModelComponent::updateAnimation(float dt) {
	deltaTime = dt;
	std::shared_ptr<animation::AnimationDescriptor> currentAnimation = currentNode->animationDescriptor;
	if (currentAnimation) {
		currentTime += currentAnimation->getFPS() * dt;
		if (currentTime > currentAnimation->getDuration()) {
			currentLoopCount++;
			if (currentNode->loopCount > 0 && currentLoopCount > currentNode->loopCount) {
				finalLoop();
				return;
			}
		}
		currentTime = fmod(currentTime, currentAnimation->getDuration());
		calculateBoneTransform(currentAnimation->getRootNode(), glm::mat4(1.0f));
	}
}

void SkeletalModelComponent::calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform) {
	std::shared_ptr<animation::AnimationDescriptor> currentAnimation = currentNode->animationDescriptor;

	std::string nodeName = node->name;
	glm::mat4 nodeTransform = node->transformation;

	Bone* Bone = currentAnimation->findBoneByName(nodeName);

	if (Bone)
	{
		Bone->update(currentTime);
		nodeTransform = Bone->transform;
	}

	glm::mat4 globalTransformation = parentTransform * nodeTransform;

	auto boneInfoMap = currentAnimation->getBoneInfoMap();
	if (boneInfoMap.find(nodeName) != boneInfoMap.end())
	{
		int index = boneInfoMap[nodeName].id;
		glm::mat4 offset = boneInfoMap[nodeName].offset;
		transformMatrices[index] = globalTransformation * offset;
	}

	for (int i = 0; i < node->childrenCount; i++)
		calculateBoneTransform(&node->children[i], globalTransformation);
}

void SkeletalModelComponent::finalLoop() {
	if (nextNode) {
		transitionToNode(nextNode);
	}
	else {
		currentNode = nullptr;
	}
}

void SkeletalModelComponent::transitionToNode(AnimationControllerNode* node) {
	currentNode = node;
	currentTime = 0;
	currentLoopCount = 0;
	nextNode = nullptr;
}

void SkeletalModelComponent::addNode() {
	AnimationControllerNode node;
	node.name = "New Node";
	node.animationDescriptor = nullptr;
	node.loopCount = 0;
	nodes.push_back(node);
}

void SkeletalModelComponent::addNoConditionTransition() {
	if (!(selectedNode)) {
		return;
	}
	NoConditionACTransition transition;
	transition.transitionTo = selectedNode;
	selectedNode->noConditionTransitions.push_back(transition);
}

void SkeletalModelComponent::addBoolACTransition() {
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

void SkeletalModelComponent::addIntACTransition() {
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

void SkeletalModelComponent::addFloatACTransition() {
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

void SkeletalModelComponent::removeNode(int id) {
	assert(nodes.size() > id);
	AnimationControllerNode* node = &nodes[id];
	for (size_t i = 0; i < nodes.size(); i++) {
		for (size_t j = 0; j < nodes[i].noConditionTransitions.size(); j++) {
			if (nodes[i].noConditionTransitions[j].transitionTo == node) {
				nodes[i].noConditionTransitions.erase(nodes[i].noConditionTransitions.begin() + j);
				j--;
			}
		}
		for (size_t j = 0; j < nodes[i].boolTransitions.size(); j++) {
			if (nodes[i].boolTransitions[j].transitionTo == node) {
				nodes[i].boolTransitions.erase(nodes[i].boolTransitions.begin() + j);
				j--;
			}
		}
		for (size_t j = 0; j < nodes[i].intTransitions.size(); j++) {
			if (nodes[i].intTransitions[j].transitionTo == node) {
				nodes[i].intTransitions.erase(nodes[i].intTransitions.begin() + j);
				j--;
			}
		}
		for (size_t j = 0; j < nodes[i].floatTransitions.size(); j++) {
			if (nodes[i].floatTransitions[j].transitionTo == node) {
				nodes[i].floatTransitions.erase(nodes[i].floatTransitions.begin() + j);
				j--;
			}
		}
	}
	nodes.erase(nodes.begin() + id);
	if (selectedNode == node) {
		selectedNode = nullptr;
	}
	if (currentNode == node) {
		currentNode = nullptr;
	}
}

void SkeletalModelComponent::removeNoConditionTransition(int id) {
	assert(selectedNode->noConditionTransitions.size() > id);
	selectedNode->noConditionTransitions.erase(selectedNode->noConditionTransitions.begin() + id);
}

void SkeletalModelComponent::removeBoolACTransition(int id) {
	assert(selectedNode->boolTransitions.size() > id);
	selectedNode->boolTransitions.erase(selectedNode->boolTransitions.begin() + id);
}

void SkeletalModelComponent::removeIntACTransition(int id) {
	assert(selectedNode->intTransitions.size() > id);
	selectedNode->intTransitions.erase(selectedNode->intTransitions.begin() + id);
}

void SkeletalModelComponent::removeFloatACTransition(int id) {
	assert(selectedNode->floatTransitions.size() > id);
	selectedNode->floatTransitions.erase(selectedNode->floatTransitions.begin() + id);
}