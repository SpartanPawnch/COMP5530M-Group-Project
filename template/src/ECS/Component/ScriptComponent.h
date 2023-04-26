#pragma once

#include <string>

#include "BaseComponent.h"

struct ScriptComponent : BaseComponent {
    ScriptComponent();
    ~ScriptComponent();
    virtual void start() override;
    virtual void update(float dt) override;
    virtual void stop() override;

    int scriptIdx = -1;
    std::string scriptUuid;
};
