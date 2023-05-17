#include "RigidBodyComponent.h"

static int baseUuid = 0;

RigidBodyComponent::RigidBodyComponent() {
    name = "Skybox Component";
    uuid = baseUuid++;
    instance = PhysicsEngine::getInstance();
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
   /* instance->deleteRidigBody(rigidBody);
    delete rigidBody;
    rigidBody = nullptr;*/
}

void RigidBodyComponent::setType(BodyType type) {
    if (rigidBody != nullptr) {
        rigidBody->setType(type);
        bodyType = type;
    }
}

void RigidBodyComponent::setPosition() {
    const Transform& transform = Transform(Vector3(position.x, position.y, position.z), Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    rigidBody->setTransform(transform);
    rigidBody->setLinearVelocity(Vector3(0, 0, 0));
    rigidBody->setAngularVelocity(Vector3(0, 0, 0));
}


