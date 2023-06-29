#pragma once

#include <vector>
#include <string>

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>

#include "../Component/BaseComponent.h"
#include "../ComponentStorage/ComponentStorage.h"

// Requires Change!!
class BaseEntity {
  public:
    BaseEntity();
    ~BaseEntity();
    virtual void start();
    void genTransform(const glm::mat4& parentMat);
    virtual void update(const glm::mat4& parentMat, float dt);
    virtual void stop();
    // void removeComponent(int uuid);

    // look for the best uuid generator
    int uuid;
    std::string name;

    int parent = -1;

    // local transform
    glm::vec3 position = glm::vec3(.0f);
    glm::quat rotation = glm::quat(1.0f, glm::vec3(.0f)); //== 0 rad
    glm::vec3 scale = glm::vec3(1.0f);

    // world-space transform
    glm::mat4 runtimeTransform = glm::mat4(1.0f);

    ComponentStorage components;
};
