#pragma once

#include <vector>

#include "BaseEntity.h"
#include "../Component/BaseComponent.h"

class ModelEntity : public BaseEntity{
public:
    ModelEntity();
    ~ModelEntity();
    virtual void start() override;
    virtual void update(float dt) override;
    virtual void stop() override;
};