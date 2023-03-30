#pragma once

#include <vector>
#include "glm/glm.hpp"


#include "BaseComponent.h"

class TransformComponent : public BaseComponent{
public:
    TransformComponent();
    ~TransformComponent();
    virtual void start() override;
    virtual void update(float dt) override;
    virtual void stop() override;
    virtual void drawInterface() override;

    int uuid;
    glm::vec3 position;
    glm::vec4 rotation;
    glm::vec3 scale;
};