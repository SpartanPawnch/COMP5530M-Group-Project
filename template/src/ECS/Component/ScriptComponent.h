#pragma once

#include <string>
#include <vector>

#include "BaseComponent.h"
#include "../EntityState/EntityState.h"
#include "../ComponentLocation/ComponentLocation.h"

struct ScriptArgument {
    // union of vals
    union {
        int _int;
        float _float;
        ComponentLocation _loc;
    } arg;
    enum ArgType {
        BOOL,
        INT,
        FLOAT,
        STRING,
        ENTITY,
        COMPONENT,
        NONE,
        ARGTYPE_MAX
    };
    std::string stringBuf; // added separately to avoid compile issues
    void* ref; // runtime ref to entity or component

    ArgType type = NONE;
    std::string key;
};

struct ScriptComponent : BaseComponent {
    ScriptComponent();
    ScriptComponent(const std::string& _name, const int _uuid);
    ~ScriptComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;

    std::string scriptPath;
    std::vector<ScriptArgument> args;
    bool valid = false;
};
