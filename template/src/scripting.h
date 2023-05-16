#pragma once
#include <string>

#include <lua.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace scripting {
    struct ScriptManager {
        ScriptManager();
        ~ScriptManager();
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

    // --- Library Side (CPP references in Lua) ---
    void pushFloatRef(lua_State* state, float* f);

    void pushVec3Ref(lua_State* state, glm::vec3* vec);

    void pushQuatRef(lua_State* state, glm::quat* quat);

    // --- Application Side (Running Lua scripts from CPP) ---

    // run lua file in path
    bool runScript(const char* path);

    // get pointer to main lua stack
    lua_State* getState();
}
