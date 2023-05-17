#include "RigidBodyComponent.h"

static int baseUuid = 0;

RigidBodyComponent::RigidBodyComponent() {
    name = "Skybox Component";
    uuid = baseUuid++;
    PhysicsEngine* instance = PhysicsEngine::getInstance();
    assert(rigidBody == nullptr);
    rigidBody = instance->createRigidBody();
}
RigidBodyComponent::RigidBodyComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}
void RigidBodyComponent::start() {
   /* PhysicsEngine* instance = PhysicsEngine::getInstance();
    assert(rigidBody == nullptr);
    rigidBody = instance->createRigidBody();*/
}
void RigidBodyComponent::update(float dt, EntityState& state) {
}
void RigidBodyComponent::stop() {
    PhysicsEngine* instance = PhysicsEngine::getInstance();
   /* instance->deleteRidigBody(rigidBody);
    delete rigidBody;
    rigidBody = nullptr;*/
}
