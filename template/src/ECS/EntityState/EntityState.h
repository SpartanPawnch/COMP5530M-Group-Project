#pragma once
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

struct EntityState {
    // local transform
    glm::vec3 position = glm::vec3(.0f);
    glm::quat rotation = glm::quat(1.0f, glm::vec3(.0f)); //== 0 rad
    glm::vec3 scale = glm::vec3(1.0f);

    // world-space transform
    glm::mat4 runtimeTransform = glm::mat4(1.0f);
};
