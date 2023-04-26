#pragma once

#include <vector>
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>

#include "BaseComponent.h"
#include "../EntityState/EntityState.h"

class TransformComponent : public BaseComponent {
  public:
    TransformComponent();
    ~TransformComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;

    int uuid;
    glm::vec3 position = glm::vec3(.0f);
    glm::quat rotation = glm::quat(1.0f, glm::vec3(.0f));
    glm::vec3 scale = glm::vec3(1.0f);
};
