#pragma once

#include <glm/mat4x4.hpp>

#include "BaseComponent.h"
#include "../../../render-engine/Camera.h"

struct CameraComponent : BaseComponent {
    CameraComponent();
    CameraComponent(const std::string& name, const int uuid);
    ~CameraComponent();
    void start() override;
    void update(float dt) override;
    void stop() override;

    static int activeUuid;

    // editable state
    float fov = 90.0f;
    glm::mat4 camMatrix = glm::mat4(1.0f);
};
