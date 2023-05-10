#include "TransformComponent.h"

static int baseUuid = 0;

TransformComponent::TransformComponent() {
    uuid = baseUuid++;
    name = "Transform Component";
}
TransformComponent::TransformComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

TransformComponent::~TransformComponent() {
    stop();
}

void TransformComponent::start() {
    // position = glm::vec3(0.0f, 0.0f, 0.0f);
    // rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    // scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void TransformComponent::update(float dt, EntityState& state) {
}

void TransformComponent::stop() {
}
