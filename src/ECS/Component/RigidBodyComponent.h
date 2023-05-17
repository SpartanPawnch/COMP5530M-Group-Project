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

    RigidBody* rigidBody=nullptr;
};