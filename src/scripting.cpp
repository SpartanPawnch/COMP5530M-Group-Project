#include "scripting.h"

#include <map>
#include <vector>
#include <fstream>

#include <cstdlib>
#include <cassert>

#include <lua.hpp>

#include "logging.h"
#include "util.h"

namespace scripting {
    static lua_State* luaState;

    static const char* floatRefMetatable = "ONO_FloatRef";

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

    // --- FloatRef Metatable --
    int luaFloatRefSet(lua_State* state) {
        // check argument count
        int argc = lua_gettop(state);
        if (argc != 2) {
            // clear stack
            lua_settop(state, 0);

            // send error
            lua_pushliteral(state,
                "FloatRef:set() - wrong number of arguments; "
                "Usage: FloatRef:set(var,f) or var:set(f)");
        }

        // first argument is the userdata
        int res = lua_getiuservalue(state, 1, 1);
        if (!res) {
            // get rid of nil on stack
            lua_settop(state, 0);

            // send error
            lua_pushliteral(state, "FloatRef:set() - missing user value in table");
            lua_error(state);
            return 0;
        }

        // convert to address
        float* f = (float*)lua_touserdata(state, -1);

        // change value
        *f = lua_tonumber(state, 2);

        // clear stack
        lua_settop(state, 0);

        return 0;
    }

    int luaFloatRefGet(lua_State* state) {
        // get userdata
        int res = lua_getiuservalue(state, 1, 1);
        if (!res) {
            // get rid of nil on stack
            lua_pop(state, 1);

            // send error
            lua_pushliteral(state, ":get() - missing user value in table");
            lua_error(state);
            return 0;
        }

        // convert to address
        float* f = (float*)lua_touserdata(state, -1);

        // empty stack
        lua_settop(state, 0);

        // return value
        lua_pushnumber(state, *f);

        return 1;
    }

    void registerFloatRefMT() {
        luaL_newmetatable(luaState, floatRefMetatable);
        // register index op - REQUIRED
        lua_pushvalue(luaState, -1);
        lua_setfield(luaState, -2, "__index");
        // register get op
        lua_pushcfunction(luaState, &luaFloatRefGet);
        lua_setfield(luaState, -2, "get");
        // register set op
        lua_pushcfunction(luaState, &luaFloatRefSet);
        lua_setfield(luaState, -2, "set");
        lua_pop(luaState, 1);
    }

    // push float
    void pushFloatRef(lua_State* state, float* f) {
        lua_newuserdatauv(state, sizeof(void*), 1);

        // add pointer to float
        lua_pushlightuserdata(state, f);
        lua_setiuservalue(state, -2, 1);

        // add metatable
        luaL_setmetatable(state, floatRefMetatable);
    }

    // push vec3
    void pushVec3Ref(lua_State* state, glm::vec3* vec) {
        lua_createtable(state, 3, 0);
        pushFloatRef(state, &vec->x);
        lua_setfield(state, -2, "x");
        pushFloatRef(state, &vec->y);
        lua_setfield(state, -2, "y");
        pushFloatRef(state, &vec->z);
        lua_setfield(state, -2, "z");
    }

    // push quaternion
    void pushQuatRef(lua_State* state, glm::quat* quat) {
        lua_createtable(state, 4, 0);
        pushFloatRef(state, &quat->x);
        lua_setfield(state, -2, "x");
        pushFloatRef(state, &quat->y);
        lua_setfield(state, -2, "y");
        pushFloatRef(state, &quat->z);
        lua_setfield(state, -2, "z");
        pushFloatRef(state, &quat->w);
        lua_setfield(state, -2, "w");
    }

    // run script from file
    bool runScript(const char* path) {
        int stackcount = lua_gettop(luaState);
        // run using dofile
        int res = luaL_dofile(luaState, path);

        // log any errors
        if (res != LUA_OK) {
            const char* err = luaL_tolstring(luaState, -1, nullptr);
            logging::logErr("LUA RUNTIME ERROR: {}\n", err);
            lua_settop(luaState, stackcount);
            return false;
        }
        return true;
    }

    lua_State* getState() {
        return luaState;
    }

    ScriptManager::ScriptManager() {
        // init default lua state and libs
        luaState = lua_newstate(&l_alloc, nullptr);
        assert(luaState != nullptr);
        luaL_openlibs(luaState);

        // setup warning handler
        lua_setwarnf(luaState, &l_warn, nullptr);

        // setup script components global
        lua_createtable(luaState, 0, 0);
        lua_setglobal(luaState, "ScriptComponents");

        // setup metatables
        registerFloatRefMT();
    }

    ScriptManager::~ScriptManager() {
        // free all lua resources
        lua_close(luaState);
        luaState = nullptr;
    }

}
