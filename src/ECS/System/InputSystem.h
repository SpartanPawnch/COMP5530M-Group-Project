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
    InputSystem(Scene* sc);
    ~InputSystem();
    void start() override;
    void update();
    void stop() override;

    void attachScene(Scene* sc);
    void detachScene();

    GLFWwindow* window;
    Scene* scene;
};