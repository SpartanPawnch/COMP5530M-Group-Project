#include "ModelComponent.h"

#include "../../model_import/model.h"

static int baseUuid = 0;

ModelComponent::ModelComponent() {
    name = "Model Component";
    uuid = baseUuid++;
}
void ModelComponent::start() {
}
void ModelComponent::update(float dt, EntityState& state) {
}
void ModelComponent::stop() {
}
