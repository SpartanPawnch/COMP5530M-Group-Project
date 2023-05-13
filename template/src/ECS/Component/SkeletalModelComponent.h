#pragma once

#include <string>
#include <map>
#include <glm/gtx/string_cast.hpp>
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
    float blendTime = 0.0f;
};

struct BoolACTransition {
    AnimationControllerNode* transitionTo;
    bool immediate; // should translate instantly or only when animation is done
    bool condition;
    bool desiredValue;
    float blendTime = 0.0f;
};
struct IntACTransition {
    AnimationControllerNode* transitionTo;
    bool immediate;
    int condition;
    int desiredValue;
    bool shouldBeGreater;
    bool shouldBeEqual;
    bool shouldBeLower;
    float blendTime = 0.0f;
};
struct FloatACTransition {
    AnimationControllerNode* transitionTo;
    bool immediate;
    float condition;
    float desiredValue;
    bool shouldBeGreater;
    bool shouldBeEqual;
    bool shouldBeLower;
    float blendTime = 0.0f;
};

struct SkeletalModelComponent : BaseComponent {
    SkeletalModelComponent();
    ~SkeletalModelComponent() {
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    void finalLoop();
    void transitionToNode(AnimationControllerNode* node, float bt = 0.0f);

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
    void calculateBoneTransform(Bone* bone, glm::mat4 parentTransform);
    void calculateBoneTransformBlended(Bone* current, Bone* previous, glm::mat4 parentTransform);
    void resetMarices();

    // model info
    std::string modelUuid = std::string("");
    std::shared_ptr<model::ModelDescriptor> modelDescriptor;

    // animation controller
    std::vector<AnimationControllerNode> nodes;
    AnimationControllerNode* previousNode;
    AnimationControllerNode* currentNode;
    AnimationControllerNode* nextNode;
    AnimationControllerNode* selectedNode;

    float currentTime;
    int currentLoopCount;
    float prevTime;
    bool isPlaying;
    bool isBlending;
    float blendFactor;
    float blendTime;
    float currentBlendTime;
    std::vector<glm::mat4> transformMatrices;
};
