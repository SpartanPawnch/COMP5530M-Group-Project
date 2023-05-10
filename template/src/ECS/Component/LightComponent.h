#pragma once
#include "BaseComponent.h"
#include <memory>
#include "../../../render-engine/LightSource.h"
struct LightComponent : BaseComponent {
    std::shared_ptr<LightDescriptor> desc;
    glm::vec3 position = glm::vec3(.0f);
    glm::vec3 ambient = glm::vec3(.2f);
    glm::vec3 diffuse = glm::vec3(.8f);
    glm::vec3 specular = glm::vec3(1.f);

    LightComponent();
    ~LightComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;
};
