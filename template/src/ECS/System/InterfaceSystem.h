#pragma once

#include <vector>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "BaseSystem.h"

#include "../Entity/BaseEntity.h"
#include "../Entity/CameraEntity.h"
#include "../Entity/ModelEntity.h"
#include "../Entity/SkeletalMeshEntity.h"


#include "../Component/BaseComponent.h"

#include "../Scene/Scene.h"

class InterfaceSystem : public BaseSystem {
public:
    InterfaceSystem(Scene* sc);
    ~InterfaceSystem();
    void start() override;
    void update(float dt) override;
    void stop() override;

    GLFWwindow* window;
    Scene* scene;
};