#pragma once
#include <lua.hpp>

namespace scripting {
    struct ScriptManager {
        ScriptManager();
        ~ScriptManager();
    };

    void registerFunction(lua_CFunction fun);
}
