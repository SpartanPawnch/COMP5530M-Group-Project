#pragma once

#include <string>

#include "BaseComponent.h"
#include "../EntityState/EntityState.h"

struct ScriptComponent : BaseComponent {
    ScriptComponent();
    ScriptComponent(const std::string& _name, const int _uuid);
    ~ScriptComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;

    std::string scriptPath;
    bool valid = false;
};
