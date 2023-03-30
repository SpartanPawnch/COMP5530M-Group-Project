#include "TransformComponent.h"

TransformComponent::TransformComponent()
{
    uuid = 999;
    name = "Transform Component";
    start();
}

TransformComponent::~TransformComponent()
{
    stop();
}

void TransformComponent::start()
{
    position = glm::vec3(0.0f, 0.0f, 0.0f);
    rotation = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    scale = glm::vec3(1.0f, 1.0f, 1.0f);
}

void TransformComponent::update(float dt)
{
}

void TransformComponent::stop()
{
}