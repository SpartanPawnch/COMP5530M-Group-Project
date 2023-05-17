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

    PhysicsEngine* instance;
    BodyType bodyType = BodyType::DYNAMIC;

    RigidBody* rigidBody=nullptr;
    glm::vec3 position=glm::vec3(0.0f);
    glm::quat rotation = glm::quat();
};