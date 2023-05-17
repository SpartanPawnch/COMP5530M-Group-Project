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
void RigidBodyComponent::addCollider()
{
    switch (currentColliderType)
    {
    case CUBE:
        createCubeCollider();
        break;
    case SPHERE:
        createSphereCollider();
        break;
    case CAPSULE:
        createCapsuleCollider();
        break;
    case MESH:
        createMeshCollider();
        break;
    default:
        break;
    }
}
void RigidBodyComponent::stop() {
   /* instance->deleteRidigBody(rigidBody);
    delete rigidBody;
    rigidBody = nullptr;*/
}

void RigidBodyComponent::createCubeCollider()
{
    const Vector3 halfExtents(1.0, 1.0, 1.0);
    Transform transform = Transform::identity();
    BoxShape* boxShape = instance->physicsCommon.createBoxShape(halfExtents);

    ColliderObject collider;
    collider.shape = ColliderTypes::CUBE;
    collider.collider = rigidBody->addCollider(boxShape, transform);

    colliders.push_back(collider);
}

void RigidBodyComponent::createSphereCollider()
{
    Transform transform = Transform::identity();
    SphereShape* sphereShape = instance->physicsCommon.createSphereShape(2.0);

    ColliderObject collider;
    collider.shape = ColliderTypes::SPHERE;
    collider.collider = rigidBody->addCollider(sphereShape, transform);

    colliders.push_back(collider);
}

void RigidBodyComponent::setCollisionMask(int index, CollisionCategories mask)
{
    colliders[index].collider->setCollisionCategoryBits(mask);
}


void RigidBodyComponent::setCollisionCollideWithMask(int index, CollisionCategories mask)
{
    colliders[index].collidesWith.push_back(mask);
    uint32_t combinedMaskBits = 0;
    for (uint32_t maskBits : colliders[index].collidesWith) {
        combinedMaskBits |= maskBits;
    }
    colliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
}

void RigidBodyComponent::removeCollisionCollideWithMask(int index, int maskIndex)
{
    if (maskIndex < colliders[index].collidesWith.size()) {
        colliders[index].collidesWith.erase(colliders[index].collidesWith.begin() + maskIndex);
    }
    uint32_t combinedMaskBits = 0;
    for (uint32_t maskBits : colliders[index].collidesWith) {
        combinedMaskBits |= maskBits;
    }
    colliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
}

void RigidBodyComponent::setMaterialBounciness(int index, float bounciness)
{
    colliders[index].materialBounciness = bounciness;
}

void RigidBodyComponent::setMaterialFrictionCoefficient(int index, float frictionCoefficient)
{
    colliders[index].materialFrictionCoefficient = frictionCoefficient;
}

void RigidBodyComponent::createCapsuleCollider()
{
    Transform transform = Transform::identity();
    CapsuleShape* capsuleShape = instance->physicsCommon.createCapsuleShape(1.0, 2.0);

    ColliderObject collider;
    collider.shape = ColliderTypes::CAPSULE;
    collider.collider = rigidBody->addCollider(capsuleShape, transform);

    colliders.push_back(collider);
}

void RigidBodyComponent::createMeshCollider()
{
    const Vector3 halfExtents(1.0, 1.0, 1.0);
    Transform transform = Transform::identity();
    BoxShape* boxShape = instance->physicsCommon.createBoxShape(halfExtents);

    ColliderObject collider;
    collider.shape = ColliderTypes::MESH;
    collider.collider = rigidBody->addCollider(boxShape, transform);

    colliders.push_back(collider);
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

void RigidBodyComponent::setGravityEnabled() {
    if (rigidBody != nullptr) {
        rigidBody->enableGravity(gravityEnabled);
    }
}

void RigidBodyComponent::applyForce()
{
    if (rigidBody != nullptr) {
        rigidBody->applyLocalForceAtCenterOfMass(Vector3(force.x, force.y, force.z));
    }
}


