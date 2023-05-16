#include "InputSystem.h"

#include <iostream>

InputSystem::InputSystem(GLFWwindow* window, Scene* sc)
{
    start(window);

    scene = sc;
}

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{
    stop();
}

InputSystem* InputSystem::getInstance()
{
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

void InputSystem::attachScene(Scene* sc)
{
    scene = sc;
}

void InputSystem::detachScene()
{
    scene = nullptr;
}

void InputSystem::glfw_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    InputSystem* is = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
    is->glfw_callback_key_press(window, key, scancode, action, mods);
}

void InputSystem::glfw_callback_key_press(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int)
{
    for (auto& entity : scene->entities) {
        for (auto& component : entity.components.vecPlayerControllerComponent) {            if (component.listeningForKey) {
                component.virtualKeys.at(component.listeningForKeyIndex).key = aKey;
                component.listeningForKey = false;
                return;
            }

            for (auto const& vk : component.virtualKeys) {
                if (vk.key == aKey) {
                    AssignAction(entity, vk.scale, vk.action);
                    //vk.action(entity, vk.scale);
                    //MoveLeftRight(entity,1.f);
                }
            }
        }
        
    }
}

void InputSystem::AssignAction(BaseEntity& entity, float scale, unsigned int action)
{
    switch (action) {
    case 0:
        MoveForwardBackward(entity, scale);
        break;
    case 1:
        MoveForwardBackward(entity, scale);
        break;
    case 2:
        MoveLeftRight(entity, scale);
        break;
    case 3:
        MoveLeftRight(entity, scale);
        break;
    case 4:
        MoveAscendDescend(entity, scale);
        break;
    case 5:
        MoveAscendDescend(entity, scale);
        break;
    }
}

void InputSystem::MoveLeftRight(BaseEntity& entity, float scale)
{
    entity.position = entity.position + glm::vec3(1.0f, 0.f, 0.f) * scale;
}


void InputSystem::MoveForwardBackward(BaseEntity& entity, float scale)
{
    entity.position = entity.position + glm::vec3(0.0f, 0.f, -1.f) * scale;
}

void InputSystem::MoveAscendDescend(BaseEntity& entity, float scale)
{
    entity.position = entity.position + glm::vec3(0.0f, 1.f, 0.f) * scale;
}

void InputSystem::start(GLFWwindow* window)
{
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, glfw_callback_wrapper);
}


void InputSystem::update()
{
   
}

void InputSystem::stop()
{
}