#pragma once

#include <vector>

#include "BaseEntity.h"
#include "../Component/BaseComponent.h"

class SkeletalMeshEntity : public BaseEntity{
public:
    SkeletalMeshEntity();
    ~SkeletalMeshEntity();
    virtual void start() override;
    virtual void update(float dt) override;
    virtual void stop() override;
};