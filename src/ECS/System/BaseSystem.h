#pragma once

#include <vector>

#include "Entity/BaseEntity.h"
#include "Component/BaseComponent.h"

class BaseSystem{
public:
    BaseSystem();
    ~BaseSystem();
    void start();
    void update(float dt);
    void stop();
};