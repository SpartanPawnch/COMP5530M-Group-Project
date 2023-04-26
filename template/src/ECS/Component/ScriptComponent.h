#pragma once

#include <string>

#include "BaseComponent.h"
#include "../EntityState/EntityState.h"

struct ScriptComponent : BaseComponent {
    ScriptComponent();
    ~ScriptComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;

    int scriptIdx = -1;
    std::string scriptUuid;
};
