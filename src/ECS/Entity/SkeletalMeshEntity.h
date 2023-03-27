#pragma once

#include <vector>

#include "Entity/BaseEntity.h"
#include "Component/BaseComponent.h"

class SkeletalMeshEntity : public BaseEntity{
public:
    SkeletalMeshEntity();
    ~SkeletalMeshEntity();
    void start();
    void update(float dt);
    void stop();

    int uuid;
    std::vector<BaseComponent> components;
};