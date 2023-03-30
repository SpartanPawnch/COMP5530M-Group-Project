#include "BaseComponent.h"

BaseComponent::BaseComponent()
{
    start();
}

BaseComponent::~BaseComponent()
{
    stop();
}

void BaseComponent::start()
{
}

void BaseComponent::update(float dt)
{
}

void BaseComponent::stop()
{
}

void BaseComponent::drawInterface() {
    ImGui::Text("Default Component has no parameters");
}