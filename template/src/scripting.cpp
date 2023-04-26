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

    static std::map<std::string, int> uuidToIdx;
    static std::vector<ScriptDescriptor> scripts;

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

        // free all loaded scripts
        clearScripts();
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

    void runScript(const char* path) {
        // run using dofile
        int res = luaL_dofile(luaState, path);

        // log any errors
        if (res != LUA_OK) {
            const char* err = luaL_tolstring(luaState, -1, nullptr);
            logging::logErr("LUA RUNTIME ERROR: {}", err);
        }
    }

    int loadScript(const char* path, const std::string& uuid) {
        std::ifstream file(path);
        if (!file.is_open()) {
            logging::logErr("Failed to open file {} for reading\n", path);
            return -1;
        }

        // dump file contents to string
        file.seekg(0, file.end);
        size_t bufSize = size_t(file.tellg());
        file.seekg(0, file.beg);
        std::vector<char> buf(bufSize + 1);
        setVec(buf, '\0');
        file.read(buf.data(), bufSize);
        file.close();

        // try to add new script
        if (uuidToIdx.count(uuid) == 0) {
            scripts.emplace_back(
                ScriptDescriptor{std::string(path), uuid, std::string(buf.data())});
            uuidToIdx[uuid] = scripts.size() - 1;
            return (scripts.size() - 1);
        }

        // replace existing otherwise
        int idx = uuidToIdx[uuid];
        scripts[idx].path = std::string(path);
        scripts[idx].uuid = uuid;
        scripts[idx].buf = std::string(buf.data());

        return idx;
    }

    inline int getScriptIdx(const std::string& uuid) {
        return uuidToIdx[uuid];
    }

    inline const ScriptDescriptor& getScript(int idx) {
        return scripts[idx];
    }

    void clearScripts() {
        uuidToIdx.clear();
        scripts.clear();
    }
}
