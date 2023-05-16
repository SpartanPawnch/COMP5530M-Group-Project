#include "BaseEntity.h"

#include <glm/gtx/transform.hpp>

BaseEntity::BaseEntity() {
    uuid = 10;
    name = "Base Entity";
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
    components.updateAll(dt);
}

void BaseEntity::stop() {
}

void BaseEntity::genTransform(const glm::mat4& parentMat) {
    runtimeTransform =
        parentMat * glm::translate(position) * glm::mat4_cast(rotation) * glm::scale(scale);
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
