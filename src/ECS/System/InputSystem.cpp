#include "InputSystem.h"

#include <iostream>
#include "../../scripting.h"

InputSystem::InputSystem(GLFWwindow* window) {
    start(window);
}

InputSystem::InputSystem() {
}

InputSystem::~InputSystem() {
}

InputSystem* InputSystem::getInstance() {
    if (instance == nullptr) {
        instance = new InputSystem();
    }
    return instance;
}

void InputSystem::glfw_callback_wrapper(GLFWwindow* window, int key, int scancode, int action,
    int mods) {
    InputSystem* is = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    is->glfw_callback_key_press(window, key, scancode, action, mods);
}

void InputSystem::glfw_callback_key_press(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction,
    int) {
    if (aAction == GLFW_PRESS && aKey >= 0 && aKey < GLFW_KEY_LAST + 1) {
        isDown[aKey] = true;

        // check for key listening
        if (scene->selectedEntity != nullptr) {
            for (size_t i = 0;
                 i < scene->selectedEntity->components.vecPlayerControllerComponent.size(); i++) {
                if (scene->selectedEntity->components.vecPlayerControllerComponent[i]
                        .listeningForKey) {
                    scene->selectedEntity->components.vecPlayerControllerComponent[i]
                        .virtualKeys[scene->selectedEntity->components
                                         .vecPlayerControllerComponent[i]
                                         .listeningForKeyIndex]
                        .key = aKey;
                    scene->selectedEntity->components.vecPlayerControllerComponent[i]
                        .listeningForKey = false;
                }
            }
        }
    }
    else if (aAction == GLFW_RELEASE && aKey >= 0 && aKey < GLFW_KEY_LAST + 1)
        isDown[aKey] = false;
}

void InputSystem::start(GLFWwindow* window) {
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, glfw_callback_wrapper);
}

// lua stuff
static const char* inputSystemModule = "ONO_Input";
static int luaGetRawKey(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 1 || !lua_isinteger(state, 1)) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_Input.getRawKey() - wrong number of arguments; "
            "Usage: ONO_Input.getRawKey(key:int)");
        lua_error(state);
        return 0;
    }
    int idx = lua_tointeger(state, 1);

    if (idx < 0 || idx > GLFW_KEY_LAST) {
        lua_settop(state, 0);
        lua_pushliteral(state, "ONO_Input.getRawKey() - key value is out of bounds");
        lua_error(state);
        return 0;
    }

    InputSystem* iSys = InputSystem::getInstance();

    lua_settop(state, 0);
    lua_pushboolean(state, int(iSys->isDown[idx]));
    return 1;
}

void InputSystem::registerLuaFunctions() {
    scripting::beginModule(2);
    scripting::registerModuleFunction("getRawKey", &luaGetRawKey);
    lua_State* state = scripting::getState();
    lua_createtable(state, 0, GLFW_KEY_LAST);
    for (int i = 0; i <= GLFW_KEY_LAST; i++) {
        const char* keyName = glfwGetKeyName(i, 0);
        if (!keyName)
            continue;

        lua_pushinteger(state, i);
        lua_setfield(state, -2, keyName);
    }
    lua_setfield(state, -2, "keyIDS");
    scripting::finalizeModule(inputSystemModule);
}
