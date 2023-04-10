#pragma once
#include <lua.hpp>

namespace scripting {
    struct ScriptManager {
        ScriptManager();
        ~ScriptManager();
    };

    void registerFunction(const char* name, lua_CFunction fun);
    void runFile(const char* path);
}
