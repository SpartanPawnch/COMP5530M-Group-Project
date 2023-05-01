#pragma once
#include <string>

#include <lua.hpp>

namespace scripting {
    struct ScriptManager {
        ScriptManager();
        ~ScriptManager();
    };

    struct ScriptDescriptor {
        std::string uuid;
        std::string buf;
        std::string path;
    };

    // --- Library Side (CPP functions in Lua) ---

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

    // --- Application Side (Running Lua scripts from CPP) ---

    // run lua file in path
    bool runScript(const char* path);

    // load lua file to internal buffer
    int loadScript(const char* path, const std::string& uuid);

    lua_State* getState();

    // get internal index corresponding to uuid
    inline int getScriptIdx(const std::string& uuid);

    // get script ref corresponding to internal index
    inline const ScriptDescriptor& getScript(int idx);

    // clear all currently loaded scripts
    void clearScripts();
}
