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

void RigidBodyComponent::createCubeCollider()
{
    const Vector3 halfExtents(1.0, 1.0, 1.0);
    Transform transform = Transform::identity();
    BoxShape* boxShape = instance->physicsCommon.createBoxShape(halfExtents);

    CubeColliderObject collider;
    collider.shape = ColliderTypes::CUBE;
    collider.colliderShape = boxShape;
    collider.collider = rigidBody->addCollider(boxShape, transform);
    collider.collider->getLocalToBodyTransform();

    cubeColliders.push_back(collider);
}

void RigidBodyComponent::createSphereCollider()
{
    Transform transform = Transform::identity();
    SphereShape* sphereShape = instance->physicsCommon.createSphereShape(1.0);

    SphereColliderObject collider;
    collider.colliderShape = sphereShape;
    collider.shape = ColliderTypes::SPHERE;
    collider.collider = rigidBody->addCollider(sphereShape, transform);

    sphereColliders.push_back(collider);
}

void RigidBodyComponent::createCapsuleCollider()
{
    Transform transform = Transform::identity();
    CapsuleShape* capsuleShape = instance->physicsCommon.createCapsuleShape(1.0, 3.0);

    CapsuleColliderObject collider;
    collider.colliderShape = capsuleShape;
    collider.shape = ColliderTypes::CAPSULE;
    collider.collider = rigidBody->addCollider(capsuleShape, transform);

    capsuleColliders.push_back(collider);
}

void RigidBodyComponent::createMeshCollider()
{
    Transform transform = Transform::identity();
    TriangleMesh* triangleMesh = instance->physicsCommon.createTriangleMesh();
    
    MeshColliderObject collider;
    collider.triangleMesh = triangleMesh;
    //collider.colliderShape = instance->physicsCommon.createConcaveMeshShape(triangleMesh);
    collider.shape = ColliderTypes::MESH;
    //collider.collider = rigidBody->addCollider(collider.colliderShape, transform);

    meshColliders.push_back(collider);
}

void RigidBodyComponent::setCollisionMask(ColliderTypes type, int index, CollisionCategories mask)
{
    switch (type)
    {
    case CUBE:
        cubeColliders[index].collider->setCollisionCategoryBits(mask);
        cubeColliders[index].category = mask;
        break;
    case SPHERE:
        sphereColliders[index].collider->setCollisionCategoryBits(mask);
        sphereColliders[index].category = mask;
        break;
    case CAPSULE:
        capsuleColliders[index].collider->setCollisionCategoryBits(mask);
        capsuleColliders[index].category = mask;
        break;
    case MESH:
        meshColliders[index].collider->setCollisionCategoryBits(mask);
        meshColliders[index].category = mask;
        break;
    default:
        break;
    }
}


void RigidBodyComponent::setCollisionCollideWithMask(ColliderTypes type, int index, CollisionCategories mask)
{
    switch (type)
    {
    case CUBE:
    {
        cubeColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : cubeColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        cubeColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    case SPHERE:
    {
        sphereColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : sphereColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        sphereColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    case CAPSULE:
    {
        capsuleColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : capsuleColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        capsuleColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    case MESH:
    {
        meshColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : meshColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        meshColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    default:
        break;
    }
}

void RigidBodyComponent::removeCollisionCollideWithMask(ColliderTypes type, int index, int maskIndex)
{
    switch (type)
    {
    case CUBE:
    {
        if (maskIndex < cubeColliders[index].collidesWith.size()) {
            cubeColliders[index].collidesWith.erase(cubeColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : cubeColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        cubeColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    case SPHERE:
    {
        if (maskIndex < sphereColliders[index].collidesWith.size()) {
            sphereColliders[index].collidesWith.erase(sphereColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : sphereColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        sphereColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    case CAPSULE:
    {
        if (maskIndex < capsuleColliders[index].collidesWith.size()) {
            capsuleColliders[index].collidesWith.erase(capsuleColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : capsuleColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        capsuleColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    case MESH:
    {
        if (maskIndex < meshColliders[index].collidesWith.size()) {
            meshColliders[index].collidesWith.erase(meshColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : meshColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        meshColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    }
        break;
    default:
        break;
    }
}

void RigidBodyComponent::setMaterialBounciness(ColliderTypes type, int index, float bounciness)
{
    switch (type)
    {
    case CUBE:
        cubeColliders[index].materialBounciness = bounciness;
        break;
    case SPHERE:
        sphereColliders[index].materialBounciness = bounciness;
        break;
    case CAPSULE:
        capsuleColliders[index].materialBounciness = bounciness;
        break;
    case MESH:
        meshColliders[index].materialBounciness = bounciness;
        break;
    default:
        break;
    }
}

void RigidBodyComponent::setMaterialFrictionCoefficient(ColliderTypes type, int index, float frictionCoefficient)
{
    switch (type)
    {
    case CUBE:
        cubeColliders[index].materialFrictionCoefficient = frictionCoefficient;
        break;
    case SPHERE:
        sphereColliders[index].materialFrictionCoefficient = frictionCoefficient;
        break;
    case CAPSULE:
        capsuleColliders[index].materialFrictionCoefficient = frictionCoefficient;
        break;
    case MESH:
        meshColliders[index].materialFrictionCoefficient = frictionCoefficient;
        break;
    default:
        break;
    }
}

void RigidBodyComponent::setCubeColliderExtents(int index, glm::vec3 extents) {
    cubeColliders[index].colliderShape->setHalfExtents(Vector3(extents.x, extents.y, extents.z));
}
void RigidBodyComponent::setSphereColliderRadius(int index, float radius) {
    sphereColliders[index].colliderShape->setRadius(radius);
}
void RigidBodyComponent::setCapsuleColliderRadiusHeight(int index, float radius, float height) {
    capsuleColliders[index].colliderShape->setRadius(radius);
    capsuleColliders[index].colliderShape->setHeight(height);
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

void RigidBodyComponent::setMeshColliderModel(int index) {
    unsigned int vertexCount = 0;
    unsigned int indexCount = 0;
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
    for (unsigned int i = 0; i < meshColliders[index].model->getMeshCount(); i++) {
        std::vector<unsigned int> meshIndices = meshColliders[index].model->getIndices(i);
        for (unsigned int j = 0; j < meshIndices.size(); j++) {
            meshIndices[j] += indexCount;
        }
        vertexCount += meshColliders[index].model->getVertexCount(i);
        indexCount += meshColliders[index].model->getIndexCount(i);
        std::vector<glm::vec3> meshVertices = meshColliders[index].model->getVertices(i);
        vertices.insert(vertices.end(), meshVertices.begin(), meshVertices.end());
        indices.insert(indices.end(), meshIndices.begin(), meshIndices.end());
    }

    float* verticesArray = new float[vertexCount*3];
    int* indicesArray = new int[indexCount];

    for (unsigned int i = 0; i < vertexCount; i++) {
        verticesArray[i*3] = vertices[i].x;
        verticesArray[i*3+1] = vertices[i].y;
        verticesArray[i*3+2] = vertices[i].z;
    }

    for (unsigned int i = 0; i < indexCount; i++) {
        indicesArray[i] = indices[i];
    }
    
    TriangleVertexArray* triangleArray =
        new TriangleVertexArray(vertexCount, verticesArray, 3 * sizeof(float), indexCount/3,
            indicesArray, 3 * sizeof(int),
            TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
        );

    Transform transform = Transform::identity();

    meshColliders[index].triangleMesh->addSubpart(triangleArray);

    meshColliders[index].colliderShape = instance->physicsCommon.createConcaveMeshShape(meshColliders[index].triangleMesh);
    meshColliders[index].collider = rigidBody->addCollider(meshColliders[index].colliderShape, transform);

    delete[] verticesArray;
    delete[] indicesArray;
}

