#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "BaseComponent.h"
#include "../../../render-engine/Camera.h"
#include "../EntityState/EntityState.h"

struct CameraComponent : BaseComponent {
    CameraComponent();
    CameraComponent(const std::string& name, const int uuid);
    ~CameraComponent();
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;
    glm::mat4 getMatrix();
    void copyToCamera(Camera& camera, const glm::mat4& runtimeTransform);
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();

    static int activeUuid;

    // TODO nicer camera editing mechanism
    //  editable state
    glm::vec3 eye = glm::vec3(.0f);
    glm::vec3 center = glm::vec3(.0f, .0f, -1.0f);
    glm::vec3 up = glm::vec3(.0f, 1.f, .0f);
    float fov = 90.0f;
};
