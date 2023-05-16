#include "ScriptComponent.h"

#include <lua.hpp>

#include "../../logging.h"
#include "../../scripting.h"
#include "../ComponentStorage/ComponentStorage.h"

static int baseUuid = 0;

ScriptComponent::ScriptComponent() {
    name = "Script Component";
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

static void pushArgs(lua_State* state, std::vector<ScriptArgument>& args) {
    // create initial table
    lua_createtable(state, 0, args.size());

    for (unsigned int i = 0; i < args.size(); i++) {
        if (args[i].key.empty())
            continue;

        // fill based on arg type
        switch (args[i].type) {
        case ScriptArgument::BOOL:
        case ScriptArgument::INT:
            lua_pushinteger(state, args[i].arg._int);
            break;
        case ScriptArgument::FLOAT:
            lua_pushnumber(state, args[i].arg._float);
            break;
        case ScriptArgument::STRING:
            lua_pushstring(state, args[i].stringBuf.c_str());
            break;
        case ScriptArgument::ENTITY:
            if (args[i].ref == nullptr) {
                lua_pushnil(state);
            }
            else {
                ((EntityState*)args[i].ref)->pushLuaTable(state);
            }
            break;
        case ScriptArgument::COMPONENT:
            ComponentStorage::pushLuaTable(args[i].ref, args[i].arg._loc, state);
            break;
        default:
            continue;
        }
        lua_setfield(state, -2, args[i].key.c_str());
    }
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
    lua_pushvalue(luaState, -2); // self
    lua_pushnumber(luaState, dt); // dt
    state.pushLuaTable(luaState); // entityState
    pushArgs(luaState, args); // args
    // call
    int err = lua_pcall(luaState, 4, 0, 0);
    if (err != LUA_OK) {
        const char* err = luaL_tolstring(luaState, -1, nullptr);
        logging::logErr("LUA RUNTIME ERROR: {}\n", err);
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
    if (state == nullptr)
        return;
    lua_getglobal(state, "ScriptComponents");
    lua_pushnil(state);
    lua_setfield(state, -2, std::to_string(uuid).c_str());
    lua_setglobal(state, "ScriptComponents");
}
