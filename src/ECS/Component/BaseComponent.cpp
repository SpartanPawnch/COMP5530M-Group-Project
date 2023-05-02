#include "BaseComponent.h"

BaseComponent::BaseComponent() {
    start();
}

BaseComponent::~BaseComponent() {
    stop();
}

void BaseComponent::start() {
}

void BaseComponent::update(float dt, EntityState& state) {
}

void BaseComponent::stop() {
}

void BaseComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
}
