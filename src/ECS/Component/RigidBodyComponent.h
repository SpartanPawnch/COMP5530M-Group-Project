#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "BaseComponent.h"
#include "../../physics_engine/physicsEngine.h"
#include "../../ai/ai.h"

//enum type for the ai interaction


struct RigidBodyComponent : BaseComponent {
    RigidBodyComponent();
    RigidBodyComponent(const std::string& name, const int uuid);
    ~RigidBodyComponent() {}
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    void setType(BodyType type);
    void setPosition();
    void setGravityEnabled();
    void applyForce();
    void createCubeCollider();
    void createSphereCollider();
    void createCapsuleCollider();
    void createMeshCollider();
    void setCollisionMask(ColliderTypes type, int index, CollisionCategories mask);
    void setCollisionCollideWithMask(ColliderTypes type, int index, CollisionCategories mask);
    void removeCollisionCollideWithMask(ColliderTypes type, int index, int maskIndex);
    void setMaterialBounciness(ColliderTypes type, int index, float bounciness);
    void setMaterialFrictionCoefficient(ColliderTypes type, int index, float frictionCoefficient);

    void setCubeColliderExtents(int index, glm::vec3 extents);
    void setSphereColliderRadius(int index, float radius);
    void setCapsuleColliderRadiusHeight(int index, float radius, float height);
    void setAIEnabled();


    PhysicsEngine* instance;
    BodyType bodyType = BodyType::DYNAMIC;
    AI_STATES aiType = AI_STATES::SLEEP;

    bool gravityEnabled = true;
    bool aiEnabled = false;

    RigidBody* rigidBody=nullptr;
    glm::vec3 position=glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.f, .0f, .0f, .0f);
    glm::vec3 force = glm::vec3(0.0f);
    
    std::vector<CubeColliderObject> cubeColliders;
    std::vector<SphereColliderObject> sphereColliders;
    std::vector<CapsuleColliderObject> capsuleColliders;
    std::vector<MeshColliderObject> meshColliders;

    //ai state.
    //keep track of this state for ai interations.
    StateMachine* stateMachine;
    void setAIState(AI_STATES state);
    //these functions are for the updateState of stateMachine.
    void performRandomMovement();
    Quaternion getRotationBasedOnForce();
    void findClosestRigidBody(RigidBody* dst, decimal radius);
    void goTowardsRigidBody();
    void moveTowardsABody(RigidBody* dst, decimal linearVelocity);
    Vector3 findDirection(Vector3 d);
    void moveTowardsABodyForce(RigidBody* dst);
    void createExplosion(Vector3 pos, decimal radius, decimal force);
    State* getStateMachineState();
    void performStateChange();
};