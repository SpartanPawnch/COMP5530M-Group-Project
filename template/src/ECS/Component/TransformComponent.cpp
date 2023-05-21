#include "TransformComponent.h"
#include "../../scripting.h"
static int baseUuid = 0;

int TransformComponent::genUuid() {
    return baseUuid++;
}

TransformComponent::TransformComponent() {
    uuid = baseUuid++;
    name = "Transform Component";
}
TransformComponent::TransformComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

TransformComponent::~TransformComponent() {
    stop();
}

void TransformComponent::start() {
    // position = glm::vec3(0.0f, 0.0f, 0.0f);
    // rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void TransformComponent::update(float dt, EntityState& state) {
}

void TransformComponent::stop() {
}

// lua stuff
static const char* componentMT = "ONO_TransformComponent";
void TransformComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pop(state, 1);
}

void TransformComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    scripting::pushVec3Ref(state, &position);
    lua_setfield(state, -2, "position");
    scripting::pushQuatRef(state, &rotation);
    lua_setfield(state, -2, "rotation");
    scripting::pushVec3Ref(state, &scale);
    lua_setfield(state, -2, "scale");
    luaL_setmetatable(state, componentMT);
}
