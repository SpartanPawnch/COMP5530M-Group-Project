#include "PlayerControllerComponent.h"
#include <array>

#include "../System/InputSystem.h"
#include "../../scripting.h"

static int baseUuid = 0;
PlayerControllerComponent::PlayerControllerComponent()
    : listeningForKey(false), listeningForKeyIndex(0) {

    name = "Player Controller Component";
    uuid = baseUuid++;
}

PlayerControllerComponent::PlayerControllerComponent(const std::string& _name, int _uuid) {
    name = _name;
    uuid = _uuid;
}

void PlayerControllerComponent::addKey() {
    VirtualKey vk;
    vk.name = "new key " + std::to_string(virtualKeys.size());
    vk.key = -1;
    vk.scale = 1.0f;
    virtualKeys.push_back(vk);
}

void PlayerControllerComponent::removeKey(int index) {
    virtualKeys.erase(virtualKeys.begin() + index);
}

void PlayerControllerComponent::update(float dt, EntityState& state) {
    std::array<float, VirtualKey::ACTIONS_MAX> actions{.0f};
    static InputSystem* iSys = InputSystem::getInstance();

    // update virtual virtual keys
    for (size_t i = 0; i < virtualKeys.size(); i++) {
        if (virtualKeys[i].action < 0 || virtualKeys[i].key < 0)
            continue;

        actions[virtualKeys[i].action] = glm ::max(actions[virtualKeys[i].action],
            iSys->isDown[virtualKeys[i].key] ? virtualKeys[i].scale : .0f);
    }

    if (!rigidBodies || rigidBodies->empty()) {
        // update position directly
        state.position = state.position +
            dt *
                glm::vec3(actions[VirtualKey::LEFT] - actions[VirtualKey::RIGHT],
                    actions[VirtualKey::UP] - actions[VirtualKey::DOWN],
                    actions[VirtualKey::FORWARD] - actions[VirtualKey::BACK]);
    }
    else {
        // apply force to rigid bodies
        for (size_t i = 0; i < rigidBodies->size(); i++) {
            (*rigidBodies)[i].force =
                glm::vec3(actions[VirtualKey::LEFT] - actions[VirtualKey::RIGHT],
                    actions[VirtualKey::UP] - actions[VirtualKey::DOWN],
                    actions[VirtualKey::FORWARD] - actions[VirtualKey::BACK]);
            (*rigidBodies)[i].applyForce();
            if (keepUpright) {
                // TODO replace with active rotation
                // glm::quat rot = (*rigidBodies)[i].rotation;
                // (*rigidBodies)[i].rotation =
                //     glm::normalize(glm::quat(rot.w, .0f, rot.x + rot.y + rot.z, .0f));
                (*rigidBodies)[i].updateRotation();
            }
        }
    }
}

// lua stuff
static const char* componentMT = "ONO_PlayerControllerComponent";
static int luaGetKey(lua_State* state) {
    // check argument count
    int argc = lua_gettop(state);
    if (argc != 2) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_PlayerControllerComponent:getKey() - wrong number of arguments; "
            "Usage: ONO_PlayerControllerComponent:getKey(var,i) or var:getKey(i)");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    PlayerControllerComponent* c = (PlayerControllerComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_PlayerControllerComponent - ptr is null", 0);
        return 0;
    }

    int idx = lua_tointegerx(state, 2, nullptr);

    // return key status
    lua_settop(state, 0);
    int key = (idx >= 0 && idx < c->virtualKeys.size()) ? c->virtualKeys[idx].key : -1;
    static InputSystem* iSys = InputSystem::getInstance();
    lua_pushboolean(state, (key >= 0 && key <= GLFW_KEY_LAST) ? int(iSys->isDown[key]) : 0);
    return 1;
}

void PlayerControllerComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pushcfunction(state, &luaGetKey);
    lua_setfield(state, -2, "getKey");
    lua_pop(state, 1);
}

void PlayerControllerComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    luaL_setmetatable(state, componentMT);
}
