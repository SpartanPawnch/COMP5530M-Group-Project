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

void pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
}
