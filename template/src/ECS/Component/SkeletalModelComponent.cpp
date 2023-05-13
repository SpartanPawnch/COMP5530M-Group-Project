#include "SkeletalModelComponent.h"

#include "../../model_import/model.h"

static int baseUuid = 0;

SkeletalModelComponent::SkeletalModelComponent() {
    name = "Skeletal Model Component";
    uuid = baseUuid++;
    currentTime = 0.0;
    currentLoopCount = 0;
    isPlaying = false;
    currentNode = nullptr;
    selectedNode = nullptr;
    nextNode = nullptr;
    blendFactor = 0.0f;
    isBlending = false;

    transformMatrices.reserve(MAX_BONES);

    for (unsigned int i = 0; i < MAX_BONES; i++) {
        transformMatrices.push_back(glm::mat4(1.0f));
    }
}
void SkeletalModelComponent::start() {
}
void SkeletalModelComponent::update(float dt, EntityState& state) {
    if (!isPlaying)
        return;

    if (!(currentNode)) {
        if (nodes.size() > 0) {
            currentNode = &nodes[0];
        }
        else {
            return;
        }
    }

    updateAnimation(dt);

    for each (NoConditionACTransition transition in currentNode->noConditionTransitions) {
        if (currentNode->loopCount > 0 && currentLoopCount >= currentNode->loopCount) {
            transitionToNode(transition.transitionTo, transition.blendTime);
            return;
        }
    }

    for each (BoolACTransition transition in currentNode->boolTransitions) {
        if (transition.condition == transition.desiredValue) {
            if (transition.immediate) {
                transitionToNode(transition.transitionTo, transition.blendTime);
                return;
            }
            else if (!(nextNode)) {
                nextNode = transition.transitionTo;
            }
        }
    }

    for each (IntACTransition transition in currentNode->intTransitions) {
        if ((transition.shouldBeLower && transition.condition < transition.desiredValue) ||
            (transition.shouldBeEqual && transition.condition == transition.desiredValue) ||
            (transition.shouldBeGreater && transition.condition > transition.desiredValue)) {
            if (transition.immediate) {
                transitionToNode(transition.transitionTo, transition.blendTime);
                return;
            }
            else if (!(nextNode)) {
                nextNode = transition.transitionTo;
            }
        }
    }

    for each (FloatACTransition transition in currentNode->floatTransitions) {
        if ((transition.shouldBeLower && transition.condition < transition.desiredValue) ||
            (transition.shouldBeEqual && transition.condition == transition.desiredValue) ||
            (transition.shouldBeGreater && transition.condition > transition.desiredValue)) {
            if (transition.immediate) {
                transitionToNode(transition.transitionTo, transition.blendTime);
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
    std::shared_ptr<animation::AnimationDescriptor> currentAnimation =
        currentNode->animationDescriptor;
    if (currentAnimation) {
        currentTime += currentAnimation->getTicksPerSecond() * dt;
        if (isBlending) {
            currentBlendTime += currentAnimation->getTicksPerSecond() *  dt;
            blendFactor = currentBlendTime / blendTime;
            if (currentBlendTime >= blendTime) {
                isBlending = false;
                blendFactor = 0.0f;
            }
        }
        if (currentTime / currentAnimation->getDuration() >= 1.0) {
            currentLoopCount++;
            if (currentNode->loopCount > 0 && currentLoopCount > currentNode->loopCount) {
                finalLoop();
                return;
            }
        }
        currentTime = fmod(currentTime, currentAnimation->getDuration());
        if (isBlending) {
            std::shared_ptr<animation::AnimationDescriptor> previousAnimation =
                previousNode->animationDescriptor;
            calculateBoneTransformBlended(currentAnimation->getRootBone(), previousAnimation->getRootBone(), glm::mat4(1.0f));
        }
        calculateBoneTransform(currentAnimation->getRootBone(), glm::mat4(1.0f));
    }
}

void SkeletalModelComponent::calculateBoneTransform(Bone* bone, glm::mat4 parentTransform) {
    std::shared_ptr<animation::AnimationDescriptor> currentAnimation =
        currentNode->animationDescriptor;

    glm::mat4 position = bone->interpolatePosition(currentTime);
    glm::mat4 rotation = bone->interpolateRotation(currentTime);
    glm::mat4 scale = bone->interpolateScale(currentTime);

    glm::mat4 globalTransformation = parentTransform * position * rotation * scale;

    /*std::cout << "Parent: " << std::endl;
    std::cout << glm::to_string(parentTransform) << std::endl;
    std::cout << "Parent------------" << std::endl;

    std::cout << "Position: " << std::endl;
    std::cout << glm::to_string(position) << std::endl;
    std::cout << "Position------------" << std::endl;

    std::cout << "Rotation: " << std::endl;
    std::cout << glm::to_string(rotation) << std::endl;
    std::cout << "Rotation-------------" << std::endl;

    std::cout << "Scale: " << std::endl;
    std::cout << glm::to_string(scale) << std::endl;
    std::cout << "Scale------------------" << std::endl;*/

    // std::cout << "Bone ID: " << bone->id << std::endl;

    if (bone->id < MAX_BONES && bone->id >= 0)
        transformMatrices[bone->id] = globalTransformation * bone->transform;

    for (Bone& child : bone->children) {
        calculateBoneTransform(&child, globalTransformation);
    }
}

void SkeletalModelComponent::calculateBoneTransformBlended(Bone* bone, Bone* previousBone, glm::mat4 parentTransform) {
    std::shared_ptr<animation::AnimationDescriptor> currentAnimation =
        currentNode->animationDescriptor;
    std::shared_ptr<animation::AnimationDescriptor> previousAnimation =
        previousNode->animationDescriptor;

    glm::vec3 position = bone->interpolatePositionDirectly(currentTime);
    glm::quat rotation = bone->interpolateRotationDirectly(currentTime);
    glm::vec3 scale = bone->interpolateScaleDirectly(currentTime);

    glm::vec3 previousPosition = previousBone->interpolatePositionDirectly(currentTime);
    glm::quat previousRotation = previousBone->interpolateRotationDirectly(currentTime);
    glm::vec3 previousScale = previousBone->interpolateScaleDirectly(currentTime);

    glm::vec3 blendedPosition = glm::mix(previousPosition, position, blendFactor);
    glm::quat blendedRotation = glm::slerp(previousRotation, rotation, blendFactor);
    glm::vec3 blendedScale = glm::mix(previousScale, scale, blendFactor);

    glm::mat4 positionMat = glm::translate(glm::mat4(1.0f), glm::vec3(blendedPosition));
    glm::mat4 rotationMat = glm::toMat4(blendedRotation);
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), glm::vec3(blendedScale));

    glm::mat4 globalTransformation = parentTransform * positionMat * rotationMat * scaleMat;

    if (bone->id < MAX_BONES && bone->id >= 0)
        transformMatrices[bone->id] = globalTransformation * bone->transform;

    for (unsigned int i = 0; i < bone->children.size(); i++) {
        calculateBoneTransformBlended(&bone->children[i], &previousBone->children[i], globalTransformation);
    }
}

void SkeletalModelComponent::finalLoop() {
    if (nextNode) {
        transitionToNode(nextNode);
    }
    else {
        currentNode = nullptr;
    }
}

void SkeletalModelComponent::transitionToNode(AnimationControllerNode* node, float bt) {
    previousNode = currentNode;
    currentNode = node;
    currentTime = 0;
    currentLoopCount = 0;
    nextNode = nullptr;
    if (bt > 0.0f)
        isBlending = true;
    else
        isBlending = false;
    blendFactor = 0.0f;
    blendTime = bt;
    currentBlendTime = 0.0f;
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

void SkeletalModelComponent::resetMarices() {
    for (unsigned int i = 0; i < MAX_BONES; i++) {
        transformMatrices[i] = glm::mat4(1.0);
    }
}
