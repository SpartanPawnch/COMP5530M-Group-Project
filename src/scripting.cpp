#include "scripting.h"

#include <lua.hpp>
#include <cstdlib>
#include <cassert>

namespace scripting {
    static lua_State* luaState;

    // Simple allocator from Lua manual
    // TODO replace with bespoke allocator
    static void* l_alloc(void* ud, void* ptr, size_t osize, size_t nsize) {
        (void)ud;
        (void)osize; /* not used */
        if (nsize == 0) {
            free(ptr);
            return NULL;
        }
        else
            return realloc(ptr, nsize);
    }

    ScriptManager::ScriptManager() {
        luaState = lua_newstate(&l_alloc, nullptr);
        assert(luaState != nullptr);
        luaL_openlibs(luaState);
    }

    ScriptManager::~ScriptManager() {
        lua_close(luaState);
    }

    void registerFunction(const char* name, lua_CFunction fun) {
        // table-based namespacing
        lua_register(luaState, name, fun);
    }

    void runFile(const char* path) {
        luaL_dofile(luaState, path);
    }

}
