#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "BaseComponent.h"
#include "../../physics_engine/physicsEngine.h"
#include <lua.hpp>
#include <memory>

struct RigidBodyWrapper {
    RigidBody* ptr;
    PhysicsEngine* instance;
    RigidBodyWrapper(RigidBody* _ptr, PhysicsEngine* _instance) : ptr(_ptr), instance(_instance) {
    }
    ~RigidBodyWrapper() {
        instance->deleteRidigBody(ptr);
    }
};

struct CollisionInfo {
    static constexpr unsigned int MAX_COLLISIONS = 32;
    bool collidedAsBody1 = false;
    bool collidedAsBody2 = false;
    int ownUuid = -1;
    std::string ownTag;
    int otherUuid1 = -1;
    int otherUuid2 = -1;
    std::string collidedTags[MAX_COLLISIONS];
    int collidedUuids[MAX_COLLISIONS];
    unsigned int collisionCount = 0;
};

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
    void updateRotation();
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
    void setLocalColliderPosition(ColliderTypes type, int index);
    void setLocalColliderRotation(ColliderTypes type, int index);
    void setTag(const char* str);
    void removeCollisionShape(ColliderTypes type, int index);

    void setCubeColliderExtents(int index, glm::vec3 extents);
    void setSphereColliderRadius(int index, float radius);
    void setCapsuleColliderRadiusHeight(int index, float radius, float height);
    void setMeshColliderModel(int index);

    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();
    static int genUuid();

    PhysicsEngine* instance;
    BodyType bodyType = BodyType::DYNAMIC;

    bool gravityEnabled = true;

    RigidBody* rigidBody = nullptr;
    glm::vec3 position = glm::vec3(0.0f);
    glm::quat rotation = glm::quat(1.f, .0f, .0f, .0f);
    glm::vec3 force = glm::vec3(0.0f);

    std::vector<CubeColliderObject> cubeColliders;
    std::vector<SphereColliderObject> sphereColliders;
    std::vector<CapsuleColliderObject> capsuleColliders;
    std::vector<MeshColliderObject> meshColliders;

    std::string tag;
    CollisionInfo* collisionInfo;
};
