#pragma once

#include <reactphysics3d/reactphysics3d.h>
using namespace reactphysics3d;

// Your event listener class 
class PhysicsEvents : public EventListener {
public:
    virtual void onContact(const CollisionCallback::CallbackData& callbackData) override;
};