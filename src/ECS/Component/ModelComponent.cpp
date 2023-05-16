#include "ModelComponent.h"

#include "../../model_import/model.h"

static int baseUuid = 0;

ModelComponent::ModelComponent() {
    name = "Model Component";
    uuid = baseUuid++;
}
ModelComponent::ModelComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

void ModelComponent::start() {
}
void ModelComponent::update(float dt, EntityState& state) {
}
void ModelComponent::stop() {
}
