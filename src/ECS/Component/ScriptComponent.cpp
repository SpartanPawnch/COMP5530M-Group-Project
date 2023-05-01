#include "ScriptComponent.h"

#include <lua.hpp>

#include "../../logging.h"
#include "../../scripting.h"

static int baseUuid = 0;

ScriptComponent::ScriptComponent() {
    name = "Camera Component";
    uuid = baseUuid++;
}

ScriptComponent::ScriptComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

ScriptComponent::~ScriptComponent() {
    stop();
}

void ScriptComponent::start() {
    lua_State* state = scripting::getState();

    lua_getglobal(state, "ScriptComponents");

    // run script to get functions
    valid = scripting::runScript(scriptPath.c_str());

    // check for errors
    if (!valid) {
        lua_pop(state, 1);
        return;
    }

    // add states to global components struct
    lua_setfield(state, -2, std::to_string(uuid).c_str());
    lua_setglobal(state, "ScriptComponents");
}

void ScriptComponent::update(float dt, EntityState& state) {
    if (!valid)
        return;

    lua_State* luaState = scripting::getState();
    int stacksize = lua_gettop(luaState);

    // get update func
    lua_getglobal(luaState, "ScriptComponents");
    lua_getfield(luaState, -1, std::to_string(uuid).c_str());
    lua_getfield(luaState, -1, "update");

    // push arguments
    lua_pushnumber(luaState, dt);

    // TODO EntityState

    // call
    int err = lua_pcall(luaState, 1, 0, 0);
    if (err != LUA_OK) {
        const char* err = luaL_tolstring(luaState, -1, nullptr);
        logging::logErr("LUA RUNTIME ERROR: {}", err);
    }

    // restore stack state
    lua_settop(luaState, stacksize);
}

void ScriptComponent::stop() {
    // only try to delete if valid
    if (!valid)
        return;

    // set table entry to nil
    lua_State* state = scripting::getState();
    lua_getglobal(state, "ScriptComponents");
    lua_pushnil(state);
    lua_setfield(state, -2, std::to_string(uuid).c_str());
    lua_setglobal(state, "ScriptComponents");
}
