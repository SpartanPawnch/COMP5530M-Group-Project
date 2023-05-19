#include "RigidBodyComponent.h"
#include "../../scripting.h"
static int baseUuid = 0;

RigidBodyComponent::RigidBodyComponent() {
    name = "RigidBody Component";
    uuid = baseUuid++;
    instance = PhysicsEngine::getInstance();
    assert(rigidBody == nullptr);
    rigidBody = instance->createRigidBody();
    rigidBody->setUserData(this);
}
RigidBodyComponent::RigidBodyComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
    instance = PhysicsEngine::getInstance();
    assert(rigidBody == nullptr);
    rigidBody = instance->createRigidBody();
    rigidBody->setUserData(this);
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

void RigidBodyComponent::createCubeCollider() {
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

void RigidBodyComponent::createSphereCollider() {
    Transform transform = Transform::identity();
    SphereShape* sphereShape = instance->physicsCommon.createSphereShape(1.0);

    SphereColliderObject collider;
    collider.colliderShape = sphereShape;
    collider.shape = ColliderTypes::SPHERE;
    collider.collider = rigidBody->addCollider(sphereShape, transform);

    sphereColliders.push_back(collider);
}

void RigidBodyComponent::createCapsuleCollider() {
    Transform transform = Transform::identity();
    CapsuleShape* capsuleShape = instance->physicsCommon.createCapsuleShape(1.0, 3.0);

    CapsuleColliderObject collider;
    collider.colliderShape = capsuleShape;
    collider.shape = ColliderTypes::CAPSULE;
    collider.collider = rigidBody->addCollider(capsuleShape, transform);

    capsuleColliders.push_back(collider);
}

void RigidBodyComponent::createMeshCollider() {
    Transform transform = Transform::identity();
    TriangleMesh* triangleMesh = instance->physicsCommon.createTriangleMesh();

    MeshColliderObject collider;
    collider.triangleMesh = triangleMesh;
    // collider.colliderShape = instance->physicsCommon.createConcaveMeshShape(triangleMesh);
    collider.shape = ColliderTypes::MESH;
    // collider.collider = rigidBody->addCollider(collider.colliderShape, transform);

    meshColliders.push_back(collider);
}

void RigidBodyComponent::setCollisionMask(ColliderTypes type, int index, CollisionCategories mask) {
    switch (type) {
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

void RigidBodyComponent::setCollisionCollideWithMask(ColliderTypes type, int index,
    CollisionCategories mask) {
    switch (type) {
    case CUBE: {
        cubeColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : cubeColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        cubeColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    case SPHERE: {
        sphereColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : sphereColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        sphereColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    case CAPSULE: {
        capsuleColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : capsuleColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        capsuleColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    case MESH: {
        meshColliders[index].collidesWith.push_back(mask);
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : meshColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        meshColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    default:
        break;
    }
}

void RigidBodyComponent::removeCollisionCollideWithMask(ColliderTypes type, int index,
    int maskIndex) {
    switch (type) {
    case CUBE: {
        if (maskIndex < cubeColliders[index].collidesWith.size()) {
            cubeColliders[index].collidesWith.erase(
                cubeColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : cubeColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        cubeColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    case SPHERE: {
        if (maskIndex < sphereColliders[index].collidesWith.size()) {
            sphereColliders[index].collidesWith.erase(
                sphereColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : sphereColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        sphereColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    case CAPSULE: {
        if (maskIndex < capsuleColliders[index].collidesWith.size()) {
            capsuleColliders[index].collidesWith.erase(
                capsuleColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : capsuleColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        capsuleColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    case MESH: {
        if (maskIndex < meshColliders[index].collidesWith.size()) {
            meshColliders[index].collidesWith.erase(
                meshColliders[index].collidesWith.begin() + maskIndex);
        }
        uint32_t combinedMaskBits = 0;
        for (uint32_t maskBits : meshColliders[index].collidesWith) {
            combinedMaskBits |= maskBits;
        }
        meshColliders[index].collider->setCollideWithMaskBits(combinedMaskBits);
    } break;
    default:
        break;
    }
}

void RigidBodyComponent::setMaterialBounciness(ColliderTypes type, int index, float bounciness) {
    switch (type) {
    case CUBE: {
        cubeColliders[index].materialBounciness = bounciness;
        Material& material = cubeColliders[index].collider->getMaterial();
        material.setBounciness(bounciness);
    } break;
    case SPHERE: {
        sphereColliders[index].materialBounciness = bounciness;
        Material& material = sphereColliders[index].collider->getMaterial();
        material.setBounciness(bounciness);
    } break;
    case CAPSULE: {
        capsuleColliders[index].materialBounciness = bounciness;
        Material& material = capsuleColliders[index].collider->getMaterial();
        material.setBounciness(bounciness);
    } break;
    case MESH: {
        meshColliders[index].materialBounciness = bounciness;
        Material& material = meshColliders[index].collider->getMaterial();
        material.setBounciness(bounciness);
    } break;
    default:
        break;
    }
}

void RigidBodyComponent::setMaterialFrictionCoefficient(ColliderTypes type, int index,
    float frictionCoefficient) {
    switch (type) {
    case CUBE: {
        cubeColliders[index].materialFrictionCoefficient = frictionCoefficient;
        Material& material = cubeColliders[index].collider->getMaterial();
        material.setFrictionCoefficient(frictionCoefficient);
    } break;
    case SPHERE: {
        sphereColliders[index].materialFrictionCoefficient = frictionCoefficient;
        Material& material = sphereColliders[index].collider->getMaterial();
        material.setFrictionCoefficient(frictionCoefficient);
    } break;
    case CAPSULE: {
        capsuleColliders[index].materialFrictionCoefficient = frictionCoefficient;
        Material& material = capsuleColliders[index].collider->getMaterial();
        material.setFrictionCoefficient(frictionCoefficient);
    } break;
    case MESH: {
        meshColliders[index].materialFrictionCoefficient = frictionCoefficient;
        Material& material = meshColliders[index].collider->getMaterial();
        material.setFrictionCoefficient(frictionCoefficient);
    } break;
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
    const Transform& transform = Transform(Vector3(position.x, position.y, position.z),
        Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    rigidBody->setTransform(transform);
    rigidBody->setLinearVelocity(Vector3(0, 0, 0));
    rigidBody->setAngularVelocity(Vector3(0, 0, 0));
}

void RigidBodyComponent::updateRotation() {
    Transform transform = rigidBody->getTransform();
    transform.setOrientation(Quaternion(rotation.x, rotation.y, rotation.z, rotation.w));
    rigidBody->setTransform(transform);
    rigidBody->setAngularVelocity(Vector3(.0f, .0f, .0f));
}

void RigidBodyComponent::setLocalColliderPosition(ColliderTypes type, int index) {
    switch (type) {
    case CUBE: {
        Transform transform = cubeColliders[index].collider->getLocalToBodyTransform();
        glm::vec3 pos = cubeColliders[index].position;
        transform.setPosition(Vector3(pos.x, pos.y, pos.z));
        cubeColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    case SPHERE: {
        Transform transform = sphereColliders[index].collider->getLocalToBodyTransform();
        glm::vec3 pos = sphereColliders[index].position;
        transform.setPosition(Vector3(pos.x, pos.y, pos.z));
        sphereColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    case CAPSULE: {
        Transform transform = capsuleColliders[index].collider->getLocalToBodyTransform();
        glm::vec3 pos = capsuleColliders[index].position;
        transform.setPosition(Vector3(pos.x, pos.y, pos.z));
        capsuleColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    case MESH: {
        Transform transform = meshColliders[index].collider->getLocalToBodyTransform();
        glm::vec3 pos = meshColliders[index].position;
        transform.setPosition(Vector3(pos.x, pos.y, pos.z));
        meshColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    default:
        break;
    }
}

void RigidBodyComponent::setLocalColliderRotation(ColliderTypes type, int index) {
    switch (type) {
    case CUBE: {
        Transform transform = cubeColliders[index].collider->getLocalToBodyTransform();
        glm::quat rot = cubeColliders[index].rotation;
        transform.setOrientation(Quaternion(rot.x, rot.y, rot.z, rot.w));
        cubeColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    case SPHERE: {
        Transform transform = sphereColliders[index].collider->getLocalToBodyTransform();
        glm::quat rot = sphereColliders[index].rotation;
        transform.setOrientation(Quaternion(rot.x, rot.y, rot.z, rot.w));
        sphereColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    case CAPSULE: {
        Transform transform = capsuleColliders[index].collider->getLocalToBodyTransform();
        glm::quat rot = capsuleColliders[index].rotation;
        transform.setOrientation(Quaternion(rot.x, rot.y, rot.z, rot.w));
        capsuleColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    case MESH: {
        Transform transform = meshColliders[index].collider->getLocalToBodyTransform();
        glm::quat rot = meshColliders[index].rotation;
        transform.setOrientation(Quaternion(rot.x, rot.y, rot.z, rot.w));
        meshColliders[index].collider->setLocalToBodyTransform(transform);
    } break;
    default:
        break;
    }
}

void RigidBodyComponent::setGravityEnabled() {
    if (rigidBody != nullptr) {
        rigidBody->enableGravity(gravityEnabled);
    }
}

void RigidBodyComponent::applyForce() {
    if (rigidBody != nullptr) {
        rigidBody->applyLocalForceAtCenterOfMass(Vector3(force.x, force.y, force.z));
    }
}

void RigidBodyComponent::setMeshColliderModel(int index) {
    if (!(meshColliders[index].collider == nullptr))
        return;
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

    float* verticesArray = new float[vertexCount * 3];
    int* indicesArray = new int[indexCount];

    for (unsigned int i = 0; i < vertexCount; i++) {
        verticesArray[i * 3] = vertices[i].x;
        verticesArray[i * 3 + 1] = vertices[i].y;
        verticesArray[i * 3 + 2] = vertices[i].z;
    }

    for (unsigned int i = 0; i < indexCount; i++) {
        indicesArray[i] = indices[i];
    }

    TriangleVertexArray* triangleArray =
        new TriangleVertexArray(vertexCount, verticesArray, 3 * sizeof(float), indexCount / 3,
            indicesArray, 3 * sizeof(int), TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
            TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE);

    Transform transform = Transform::identity();

    meshColliders[index].triangleMesh->addSubpart(triangleArray);

    meshColliders[index].colliderShape =
        instance->physicsCommon.createConcaveMeshShape(meshColliders[index].triangleMesh);
    meshColliders[index].collider =
        rigidBody->addCollider(meshColliders[index].colliderShape, transform);

    delete[] verticesArray;
    delete[] indicesArray;
}

// lua stuff
static const char* componentMT = "ONO_RigidBodyComponent";
static int luaHandleCollision(lua_State* state) {
    // check argument count
    int argc = lua_gettop(state);
    if (argc != 1) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:handleCollision() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent:handleCollision(var) or var:handleCollision()");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    int res = int(c->collidedAsBody1 || c->collidedAsBody2);

    lua_settop(state, 0);
    lua_pushboolean(state, res);
    c->collidedAsBody1 = false;
    c->collidedAsBody2 = false;
    return 1;
}

static int luaGetVelocity(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 1) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:getVelocity() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent:getVelocity(var) or var:getVelocity()");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    Vector3 res = c->rigidBody->getLinearVelocity();

    lua_settop(state, 0);
    lua_createtable(state, 0, 0);
    lua_pushnumber(state, res.x);
    lua_setfield(state, -2, "x");
    lua_pushnumber(state, res.y);
    lua_setfield(state, -2, "y");
    lua_pushnumber(state, res.z);
    lua_setfield(state, -2, "z");
    return 1;
}

static int luaGetPosition(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 1) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:getPosition() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent:getPosition(var) or var:getPosition()");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    Vector3 res = c->rigidBody->getTransform().getPosition();

    lua_settop(state, 0);
    lua_createtable(state, 0, 0);
    lua_pushnumber(state, res.x);
    lua_setfield(state, -2, "x");
    lua_pushnumber(state, res.y);
    lua_setfield(state, -2, "y");
    lua_pushnumber(state, res.z);
    lua_setfield(state, -2, "z");
    return 1;
}

static int luaGetRotation(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 1) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:getRotation() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent.getRotation(var) or var:getRotation()");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    Quaternion res = c->rigidBody->getTransform().getOrientation();

    lua_settop(state, 0);
    lua_createtable(state, 0, 0);
    lua_pushnumber(state, res.x);
    lua_setfield(state, -2, "x");
    lua_pushnumber(state, res.y);
    lua_setfield(state, -2, "y");
    lua_pushnumber(state, res.z);
    lua_setfield(state, -2, "z");
    lua_pushnumber(state, res.w);
    lua_setfield(state, -2, "w");
    return 1;
}

static int luaSetVelocity(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 4) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:setVelocity() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent:setVelocity(var,x,y,z) or var:setVelocity(x,y,z)");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    Vector3 velocity =
        Vector3(lua_tonumber(state, 2), lua_tonumber(state, 3), lua_tonumber(state, 4));

    c->rigidBody->setLinearVelocity(velocity);

    lua_settop(state, 0);
    return 0;
}

static int luaSetPosition(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 4) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:setPosition() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent.setPosition(var,x,y,z) or var:setPosition(x,y,z)");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    Vector3 position =
        Vector3(lua_tonumber(state, 2), lua_tonumber(state, 3), lua_tonumber(state, 4));

    Transform tr = c->rigidBody->getTransform();
    tr.setPosition(position);

    c->rigidBody->setTransform(tr);

    lua_settop(state, 0);
    return 0;
}

static int luaSetRotation(lua_State* state) {
    int argc = lua_gettop(state);
    if (argc != 5) {
        // clear stack
        lua_settop(state, 0);

        // send error
        lua_pushliteral(state,
            "ONO_RigidBodyComponent:setRotation() - wrong number of arguments; "
            "Usage: ONO_RigidBodyComponent.setRotation(var,w,x,y,z) or var:setRotation(w,x,y,z)");
        lua_error(state);
        return 0;
    }
    lua_getfield(state, 1, "ptr");
    RigidBodyComponent* c = (RigidBodyComponent*)lua_touserdata(state, -1);
    if (!c) {
        lua_settop(state, 0);
        lua_warning(state, "ONO_RigidBodyComponent - ptr is null", 0);
        return 0;
    }

    Quaternion rotation = Quaternion(lua_tonumber(state, 3), lua_tonumber(state, 4),
        lua_tonumber(state, 5), lua_tonumber(state, 2));

    Transform tr = c->rigidBody->getTransform();
    tr.setOrientation(rotation);

    c->rigidBody->setTransform(tr);
    // TODO don't do automatically, separate function
    c->rigidBody->setAngularVelocity(Vector3(.0f, .0f, .0f));

    lua_settop(state, 0);
    return 0;
}

void RigidBodyComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pushcfunction(state, &luaHandleCollision);
    lua_setfield(state, -2, "handleCollision");
    lua_pushcfunction(state, &luaGetVelocity);
    lua_setfield(state, -2, "getVelocity");
    lua_pushcfunction(state, &luaSetVelocity);
    lua_setfield(state, -2, "setVelocity");
    lua_pushcfunction(state, &luaGetPosition);
    lua_setfield(state, -2, "getPosition");
    lua_pushcfunction(state, &luaSetPosition);
    lua_setfield(state, -2, "setPosition");
    lua_pushcfunction(state, &luaGetRotation);
    lua_setfield(state, -2, "getRotation");
    lua_pushcfunction(state, &luaSetRotation);
    lua_setfield(state, -2, "setRotation");
    lua_pop(state, 1);
}

void RigidBodyComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    luaL_setmetatable(state, componentMT);
}
