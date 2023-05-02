#pragma once
#include "BaseSystem.h"
#include "../Component/ControllerComponent.h"
#include "../Component/PlayerControllerComponent.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../Scene/Scene.h"

class InputSystem : public BaseSystem {
public:
    InputSystem();
    InputSystem(GLFWwindow* window, Scene* sc);
    ~InputSystem();

    void start(GLFWwindow* window);
    void update();
    void stop() override;

    // pointer to single instance of RenderEngine
    static InputSystem* getInstance();
    static InputSystem* instance;
    bool initialized = false;
   

    void attachScene(Scene* sc);
    void detachScene();

    static void glfw_callback_wrapper(GLFWwindow* window, int key, int scancode, int action, int mods);

    void glfw_callback_key_press(GLFWwindow* aWindow, int aKey, int aScanCode, int aAction, int /*aModifierFlags*/);
    //GLFWwindow* window;
    Scene* scene;

    void AssignAction(BaseEntity& entity, float scale, unsigned int action);

    static void MoveLeftRight(BaseEntity& entity, float scale);
    static void MoveForwardBackward(BaseEntity& entity, float scale);
    static void MoveAscendDescend(BaseEntity& entity, float scale);

};