#include "LightComponent.h"
static int baseUuid = 0;
LightComponent::LightComponent() {
    uuid = baseUuid++;
}
LightComponent::~LightComponent() {
}
void LightComponent::start() {
}
void LightComponent::update(float dt, EntityState& state) {
}
void LightComponent::stop() {
}
