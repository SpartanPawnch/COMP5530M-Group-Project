#pragma once

#include <string>
#include <map>

#include "BaseComponent.h"
#include "../../asset_import/model.h"
#include "../../asset_import/animation.h"
#include "../../model_import/animation.h"
#include "../EntityState/EntityState.h"

#define MAX_BONES 100

struct NoConditionACTransition;
struct BoolACTransition;
struct IntACTransition;
struct FloatACTransition;

struct AnimationControllerNode {
    std::string name;
    std::string animationUuid = std::string("");
    std::shared_ptr<animation::AnimationDescriptor> animationDescriptor;
    unsigned int loopCount; // 0 = infinite loop, 1 = play once, 2 = play twice, etc...
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
    bool immediate; // should translate instantly or only when animation is done
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

struct SkeletalModelComponent : BaseComponent {
    SkeletalModelComponent() {
        name = "Skeletal Model Component";
        currentTime = 0.0;
        currentLoopCount = 0;
        currentNode = nullptr;
        selectedNode = nullptr;

        transformMatrices.reserve(MAX_BONES);

        for (unsigned int i = 0; i < MAX_BONES; i++) {
            transformMatrices.push_back(glm::mat4(1.0f));
        }
    }
    ~SkeletalModelComponent() {
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    void finalLoop();
    void transitionToNode(AnimationControllerNode* node);

    void addNode();
    void addNoConditionTransition();
    void addBoolACTransition();
    void addIntACTransition();
    void addFloatACTransition();
    void removeNode(int id);
    void removeNoConditionTransition(int id);
    void removeBoolACTransition(int id);
    void removeIntACTransition(int id);
    void removeFloatACTransition(int id);

    void updateAnimation(float dt);
    void calculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform);

    // model info
    std::string modelUuid = std::string("");
    std::shared_ptr<model::ModelDescriptor> modelDescriptor;

    // animation controller
    std::vector<AnimationControllerNode> nodes;
    AnimationControllerNode* currentNode;
    AnimationControllerNode* nextNode;
    AnimationControllerNode* selectedNode;

    float currentTime;
    int currentLoopCount;
    float deltaTime;
    bool isPlaying;
    std::vector<glm::mat4> transformMatrices;
};
