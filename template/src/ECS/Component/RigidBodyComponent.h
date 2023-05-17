#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "BaseComponent.h"
#include "../../physics_engine/physicsEngine.h"

struct RigidBodyComponent : BaseComponent {
    RigidBodyComponent();
    RigidBodyComponent(const std::string& name, const int uuid);
    ~RigidBodyComponent() {
        
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    void setType(BodyType type);
    void setPosition();
    void setGravityEnabled();
    void applyForce();
    void addCollider();
    void createCubeCollider();
    void createSphereCollider();
    void createCapsuleCollider();
    void createMeshCollider();
    void setCollisionMask(int index, CollisionCategories mask);
    void setCollisionCollideWithMask(int index, CollisionCategories mask);
    void removeCollisionCollideWithMask(int index, int maskIndex);
    void setMaterialBounciness(int index, float bounciness);
    void setMaterialFrictionCoefficient(int index, float frictionCoefficient);


    PhysicsEngine* instance;
    BodyType bodyType = BodyType::DYNAMIC;

    bool gravityEnabled = true;

    RigidBody* rigidBody=nullptr;
    glm::vec3 position=glm::vec3(0.0f);
    glm::quat rotation = glm::quat();
    glm::vec3 force = glm::vec3(0.0f);
    
    ColliderTypes currentColliderType;
    std::vector<ColliderObject> colliders;
};