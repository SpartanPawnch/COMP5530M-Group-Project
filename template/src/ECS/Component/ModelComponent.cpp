#include "ModelComponent.h"

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

void ModelComponent::readMaterials() {
    materials.clear();
    MaterialSystem* instance = MaterialSystem::getInstance();
    if (modelDescriptor) {
        materials.resize(modelDescriptor->getMeshCount());
        for (uint32_t i = 0; i < modelDescriptor->getMeshCount(); i++) {
            materials[i] = instance->getActiveMaterial(modelDescriptor->getMeshMaterialName(i));
        }
    }
}
