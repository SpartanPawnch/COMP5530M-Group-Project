#pragma once
#include <lua.hpp>

namespace scripting {
    struct ScriptManager {
        ScriptManager();
        ~ScriptManager();
    };

    // start creating lua module (global table of functions).
    // numMembersHint should be equal to number of functions defined
    // wrong values will work, but may impact performance
    void beginModule(int numMembersHint);

    // register lua function to current module
    void registerModuleFunction(const char* name, lua_CFunction fun);

    // finalize lua module and give it a name
    void finalizeModule(const char* name);

    // register global function outside of any module
    void registerGlobalFunction(const char* name, lua_CFunction fun);

    // run lua file in path
    void runFile(const char* path);
}
