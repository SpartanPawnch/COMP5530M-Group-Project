#pragma once

#include <vector>

#include "Entity/BaseEntity.h"
#include "Component/BaseComponent.h"

class CameraEntity : public BaseEntity{
public:
    CameraEntity();
    ~CameraEntity();
    void start();
    void update(float dt);
    void stop();

    int uuid;
    std::vector<BaseComponent> components;
};