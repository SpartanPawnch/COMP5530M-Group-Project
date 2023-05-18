#include "Scene.h"

#include <glm/mat4x4.hpp>

#include "../../scripting.h"

static int luaCopyEntityByUuid(lua_State* state) {
    // check argument count
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:copyEntityByUuid() - wrong number of arguments; "
            "Usage: var:copyEntityByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:copyEntityByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);

    // check uuid validity
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:copyEntityByUuid() - entity not found", 0);
        return 0;
    }

    BaseEntity copy = scene->entities[scene->uuidToIdx[uuid]];
    copy.uuid = -1;

    scene->addEntity(copy);

    // return new uuid
    lua_settop(state, 0);
    lua_pushinteger(state, scene->entities.back().uuid);

    return 1;
}

static int luaGetEntityStateByUuid(lua_State* state) {
    // check argument count
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:getEntityStateByUuid() - wrong number of arguments; "
            "Usage: var:getEntityStateByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:getEntityStateByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);

    // check uuid validity
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:getEntityState() - entity not found", 0);
        return 0;
    }

    // return entity state
    lua_settop(state, 0);
    scene->entities[scene->uuidToIdx[uuid]].state.pushLuaTable(state);

    return 1;
}

static int luaActivateEntityByUuid(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:activateEntityByUuid() - wrong number of arguments; "
            "Usage: var:activateEntityByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:activateEntityByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:activateEntityByUuid() - entity not found", 0);
        return 0;
    }

    scene->entities[scene->uuidToIdx[uuid]].active = true;

    lua_settop(state, 0);
    return 0;
}

static int luaDeactivateEntityByUuid(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:deactivateEntityByUuid() - wrong number of arguments; "
            "Usage: var:deactivateEntityByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:deactivateEntityByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:deactivateEntityByUuid() - entity not found", 0);
        return 0;
    }

    scene->entities[scene->uuidToIdx[uuid]].active = false;

    lua_settop(state, 0);
    return 0;
}

static int luaShowEntityByUuid(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:showEntityByUuid() - wrong number of arguments; "
            "Usage: var:showEntityByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:showEntityByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:showEntityByUuid() - entity not found", 0);
        return 0;
    }

    scene->entities[scene->uuidToIdx[uuid]].visible = true;

    lua_settop(state, 0);
    return 0;
}

static int luaHideEntityByUuid(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:hideEntityByUuid() - wrong number of arguments; "
            "Usage: var:hideEntityByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:hideEntityByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:CopyEntity() - entity not found", 0);
        return 0;
    }

    scene->entities[scene->uuidToIdx[uuid]].visible = false;

    lua_settop(state, 0);
    return 0;
}

static int luaRemoveEntityByUuid(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Scene:removeEntityByUuid() - wrong number of arguments; "
            "Usage: var:removeEntityByUuid(uuid)");
        lua_error(state);
        return 0;
    }

    int res = lua_getiuservalue(state, 1, 1);
    if (!res) {
        // get rid of nil on stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state, "ONO_Scene:removeEntityByUuid() - missing user value in table");
        lua_error(state);
        return 0;
    }

    Scene* scene = (Scene*)lua_touserdata(state, -1);
    int uuid = lua_tointeger(state, 2);
    if (scene->uuidToIdx.count(uuid) == 0) {
        lua_settop(state, 0);
        lua_warning(state, "WARNING: ONO_Scene:removeEntityByUuid() - entity not found", 0);
        return 0;
    }

    scene->removeEntityByIdx(scene->uuidToIdx[uuid]);

    lua_settop(state, 0);
    return 0;
}

static const char* luaMetatable = "ONO_Scene";
void Scene::registerLuaTable() {
    // create metatable
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, luaMetatable);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pushcfunction(state, &luaCopyEntityByUuid);
    lua_setfield(state, -2, "copyEntityByUuid");
    lua_pushcfunction(state, &luaRemoveEntityByUuid);
    lua_setfield(state, -2, "removeEntityByUuid");
    lua_pushcfunction(state, &luaActivateEntityByUuid);
    lua_setfield(state, -2, "activateEntityByUuid");
    lua_pushcfunction(state, &luaDeactivateEntityByUuid);
    lua_setfield(state, -2, "deactivateEntityByUuid");
    lua_pushcfunction(state, &luaShowEntityByUuid);
    lua_setfield(state, -2, "showEntityByUuid");
    lua_pushcfunction(state, &luaHideEntityByUuid);
    lua_setfield(state, -2, "hideEntityByUuid");
    lua_pushcfunction(state, &luaGetEntityStateByUuid);
    lua_setfield(state, -2, "getEntityStateByUuid");

    lua_pop(state, 1);

    // create global
    lua_newuserdatauv(state, sizeof(void*), 1);
    lua_pushlightuserdata(state, this);
    lua_setiuservalue(state, -2, 1);
    luaL_setmetatable(state, luaMetatable);
    lua_setglobal(state, "ONO_currentScene");
}

Scene::Scene() {
    selectedEntity = nullptr;
}

Scene::~Scene() {
    stop();
}

void Scene::start() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities[i].start();
    }
}

void Scene::updatePositions() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities[i].genTransform(entities[i].parent < 0
                ? glm::mat4(1.0f)
                : entities[entities[i].parent].state.runtimeTransform);
    }
}

void Scene::updateReferences() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        std::vector<ScriptComponent>& scripts = entities[i].components.vecScriptComponent;
        for (unsigned int j = 0; j < scripts.size(); j++) {
            for (unsigned int k = 0; k < scripts[j].args.size(); k++) {
                // update entity pointer
                if (scripts[j].args[k].type == ScriptArgument::ENTITY) {
                    int idx = uuidToIdx.count(scripts[j].args[k].arg._int) > 0
                        ? uuidToIdx[scripts[j].args[k].arg._int]
                        : -1;
                    scripts[j].args[k].ref =
                        (idx >= 0 && idx < entities.size()) ? &entities[idx].state : nullptr;
                }
                // update component pointer
                else if (scripts[j].args[k].type == ScriptArgument::COMPONENT) {
                    ComponentLocation& loc = scripts[j].args[k].arg._loc;
                    // handle invalid entity
                    if (uuidToIdx.count(loc.entityUuid) == 0) {
                        scripts[j].args[k].ref = nullptr;
                        continue;
                    }
                    scripts[j].args[k].ref =
                        entities[uuidToIdx[loc.entityUuid]].components.getProtectedPtr(loc);
                }
            }
        }
    }
}

void Scene::fixDescriptors(int entityUuid, ComponentLocation::CompType deletedType,
    int deletedIdx) {
    for (size_t i = 0; i < entities.size(); i++) {
        std::vector<ScriptComponent>& scripts = entities[i].components.vecScriptComponent;
        for (size_t j = 0; j < scripts.size(); j++) {
            for (unsigned int k = 0; k < scripts[j].args.size(); k++) {
                if (scripts[j].args[k].type == ScriptArgument::COMPONENT &&
                    scripts[j].args[k].arg._loc.type == deletedType &&
                    scripts[j].args[k].arg._loc.entityUuid == entityUuid) {
                    if (scripts[j].args[k].arg._loc.componentIdx > deletedIdx) {
                        scripts[j].args[k].arg._loc.componentIdx--;
                    }
                    else if (scripts[j].args[k].arg._loc.componentIdx == deletedIdx) {
                        scripts[j].args[k].arg._loc.componentIdx = -1;
                    }
                }
            }
        }
    }
}

void Scene::update(float dt) {
    updateReferences();
    for (unsigned int i = 0; i < entities.size(); i++) {
        glm::mat4 parentTransform = entities[i].parent < 0
            ? glm::mat4(1.0f)
            : entities[entities[i].parent].state.runtimeTransform;
        entities[i].update(parentTransform, dt);
    }
}

void Scene::stop() {
}

void Scene::addEntity(const BaseEntity& entity) {
    entities.emplace_back(entity);
    // add uuid to entity
    if (entity.uuid == -1)
        entities.back().uuid = BaseEntity::genUuid();
    uuidToIdx[entities.back().uuid] = entities.size() - 1;
}

void Scene::addChild(const BaseEntity& entity, int parentIdx) {
    addEntity(entity);
    setParent(entities.size() - 1, parentIdx);
}

void Scene::removeEntityByIdx(int idx) {
    // remove from map
    uuidToIdx.erase(entities[idx].uuid);

    // move to back
    int parentIdx = entities[idx].parent;
    for (int i = idx; i < entities.size() - 1; i++) {
        if (entities[i + 1].parent == idx)
            entities[i + 1].parent = parentIdx;

        else if (entities[i + 1].parent > idx)
            entities[i + 1].parent--;

        std::swap(entities[i], entities[i + 1]);
    }

    // delete
    entities.pop_back();
}

void Scene::selectEntity(BaseEntity* entity) {
    selectedEntity = entity;
}

void Scene::setParent(int childIdx, int parentIdx) {
    // edit parent index
    entities[childIdx].parent = parentIdx;

    // reorganize array
    // swap right if needed
    int newIdx = childIdx;
    if (newIdx < parentIdx || parentIdx < 0) {
        uuidToIdx[entities[childIdx].uuid] = parentIdx < 0 ? entities.size() - 1 : parentIdx;
        for (; newIdx < parentIdx || (parentIdx < 0 && newIdx < entities.size() - 1); newIdx++) {
            // correct parent if needed
            if (entities[newIdx + 1].parent > childIdx)
                entities[newIdx + 1].parent--;
            // fix uuidToIdx
            uuidToIdx[entities[newIdx + 1].uuid]--;
            std::swap(entities[newIdx], entities[newIdx + 1]);
        }
        entities[newIdx].parent = parentIdx >= 0 ? newIdx - 1 : -1;

        // handle children
        while (entities[childIdx].parent == childIdx) {
            setParent(childIdx, newIdx);
            newIdx--;
        }
    }
    else {
        uuidToIdx[entities[childIdx].uuid] = parentIdx + 1;
        // swap left if needed
        for (; newIdx > parentIdx + 1; newIdx--) {
            // correct parent if needed
            if (entities[newIdx - 1].parent > parentIdx)
                entities[newIdx - 1].parent++;
            uuidToIdx[entities[newIdx - 1].uuid]++;
            std::swap(entities[newIdx], entities[newIdx - 1]);
        }

        // handle children
        for (int i = childIdx + 1; i < entities.size() && entities[i].parent == childIdx; i++) {
            setParent(i, newIdx);
        }
    }
}
