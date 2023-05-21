#pragma once

#include <vector>
#include <string>
#include <lua.hpp>

#include "../EntityState/EntityState.h"

class BaseComponent {
  public:
    BaseComponent();
    BaseComponent(const std::string& name, const int uuid);
    ~BaseComponent();
    virtual void start();
    virtual void update(float dt, EntityState& state);
    virtual void stop();
    static int genUuid();
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable() {
    }

    int uuid;
    std::string name;
};
