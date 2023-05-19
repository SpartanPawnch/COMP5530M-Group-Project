#pragma once
#include <array>

#include "../Scene/Scene.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

class InputSystem {
  public:
    InputSystem();
    InputSystem(GLFWwindow* window);
    ~InputSystem();

    void start(GLFWwindow* window);

    // pointer to single instance of RenderEngine
    static InputSystem* getInstance();
    static InputSystem* instance;
    bool initialized = false;

    static void glfw_callback_wrapper(GLFWwindow* window, int key, int scancode, int action,
        int mods);

    static void registerLuaFunctions();

    void glfw_callback_key_press(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction,
        int /*aModifierFlags*/);
    std::array<bool, GLFW_KEY_LAST + 1> isDown{false};
    Scene* scene;
};
