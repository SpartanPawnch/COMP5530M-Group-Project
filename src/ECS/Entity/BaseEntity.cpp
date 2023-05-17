#include "BaseEntity.h"

#include <glm/gtx/transform.hpp>

static int baseUuid = 0;

int BaseEntity::genUuid() {
    return baseUuid++;
}

BaseEntity::BaseEntity() {
    name = "Base Entity";

    // set speeds
    this->defaultSpeed = 1.0f;
    this->highSpeed = 10.0f;
    this->lowSpeed = 0.3f;
    this->movementSpeed = this->defaultSpeed;
}

BaseEntity::BaseEntity(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

BaseEntity::~BaseEntity() {
    stop();
    components.clearAll();
}

void BaseEntity::start() {
    components.startAll();
}

void BaseEntity::update(const glm::mat4& parentMat, float dt) {
    genTransform(parentMat);
    components.updateAll(dt, state);
}

void BaseEntity::stop() {
}

void BaseEntity::genTransform(const glm::mat4& parentMat) {
    state.runtimeTransform = parentMat * glm::translate(state.position) *
        glm::mat4_cast(state.rotation) * glm::scale(state.scale);
    state.parent = parentMat;
}
// void BaseEntity::removeComponent(int uuid)
// {
//     for (unsigned int i = 0; i < components.size(); i++)
//     {
//         if (components[i]->uuid == uuid)
//         {
//             components.erase(components.begin() + i);
//         }
//     }
// }
