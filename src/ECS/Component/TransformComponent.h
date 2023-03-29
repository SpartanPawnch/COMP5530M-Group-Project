#pragma once

#include <vector>
#include "glm/glm.hpp"


#include "BaseComponent.h"

class TransformComponent : public BaseComponent{
public:
    TransformComponent();
    ~TransformComponent();
    void start();
    void update(float dt);
    void stop();

    int uuid;
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;
};