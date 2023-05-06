#pragma once
#include "BaseComponent.h"
struct LightComponent : BaseComponent {
    LightComponent();
    ~LightComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;
};
