#include "EntityState.h"

#include "../../scripting.h"

void EntityState::pushLuaTable(lua_State* luaState) {
    lua_createtable(luaState, 2, 0);
    // position
    scripting::pushVec3Ref(luaState, &position);
    lua_setfield(luaState, -2, "position");

    // rotation
    scripting::pushQuatRef(luaState, &rotation);
    lua_setfield(luaState, -2, "rotation");

    // scale
    scripting::pushVec3Ref(luaState, &scale);
    lua_setfield(luaState, -2, "scale");
}
