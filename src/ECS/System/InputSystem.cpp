#include "InputSystem.h"

#include <iostream>

InputSystem::InputSystem(Scene* sc)
{
    start();
}

InputSystem::~InputSystem()
{
    stop();
}

void InputSystem::attachScene(Scene* sc)
{
    scene = sc;
}

void InputSystem::detachScene()
{
    scene = nullptr;
}

void InputSystem::start()
{
}

void InputSystem::update()
{
    //each system will have a different update method
    if (scene != nullptr) {
        for (unsigned int i = 0; i < scene->entities.size(); i++) {
            for (unsigned int j = 0; j < scene->entities[i].components.vecPlayerControllerComponent.size(); j++) {
                std::cout << "hi" << std::endl;
            }
        }
    }
   
}

void InputSystem::stop()
{
}