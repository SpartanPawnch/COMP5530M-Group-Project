#include "BaseComponent.h"

static int baseUuid = 0;

BaseComponent::BaseComponent() {
    uuid = baseUuid++;
}

BaseComponent::~BaseComponent() {
    stop();
}

BaseComponent::BaseComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
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
