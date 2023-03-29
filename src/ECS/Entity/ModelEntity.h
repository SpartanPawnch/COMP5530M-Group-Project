#pragma once

#include <vector>

#include "BaseEntity.h"
#include "../Component/BaseComponent.h"

class ModelEntity : public BaseEntity{
public:
    ModelEntity();
    ~ModelEntity();
    void start();
    void update(float dt);
    void stop();

    int uuid;
    std::vector<BaseComponent> components;
};