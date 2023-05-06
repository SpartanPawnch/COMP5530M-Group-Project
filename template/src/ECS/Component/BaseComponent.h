#pragma once

#include <vector>
#include <string>
#include <lua.hpp>

#include "../EntityState/EntityState.h"

class BaseComponent {
  public:
    BaseComponent();
    ~BaseComponent();
    virtual void start();
    virtual void update(float dt, EntityState& state);
    virtual void stop();
    void pushLuaTable(lua_State* state);

    int uuid;
    std::string name;
};
