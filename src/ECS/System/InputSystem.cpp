#include "InputSystem.h"

#include <iostream>

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
        std::cout << "InputSystem created now." << std::endl;
    }
    else {
        std::cout << "InputSystem has been created previously. Returning previous instance."
                  << std::endl;
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
