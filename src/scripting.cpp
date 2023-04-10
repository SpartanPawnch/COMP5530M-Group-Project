#include "scripting.h"

#include <lua.hpp>
#include <cstdlib>
#include <cassert>

#include "logging.h"

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

    static void l_warn(void* ud, const char* msg, int tocont) {
        logging::logWarn("LUA WARN: {}\n", msg);
    }

    ScriptManager::ScriptManager() {
        // init default lua state and libs
        luaState = lua_newstate(&l_alloc, nullptr);
        assert(luaState != nullptr);
        luaL_openlibs(luaState);

        // setup warning handler
        lua_setwarnf(luaState, &l_warn, nullptr);
    }

    ScriptManager::~ScriptManager() {
        // free all lua resources
        lua_close(luaState);
    }

    // just create table
    void beginModule(int numMembersHint) {
        lua_createtable(luaState, 0, numMembersHint);
    }

    // register function as global
    void registerGlobalFunction(const char* name, lua_CFunction fun) {
        lua_register(luaState, name, fun);
    }

    // add to current table
    void registerModuleFunction(const char* name, lua_CFunction fun) {
        lua_pushcfunction(luaState, fun);
        lua_setfield(luaState, -2, name);
    }

    // set global to active table
    void finalizeModule(const char* name) {
        lua_setglobal(luaState, name);
    }

    void runFile(const char* path) {
        int res = luaL_dofile(luaState, path);
        if (res != LUA_OK) {
            const char* err = luaL_tolstring(luaState, -1, nullptr);
            logging::logErr("LUA RUNTIME ERROR: {}", err);
        }
    }

}
