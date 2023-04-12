#include "BaseEntity.h"

BaseEntity::BaseEntity() {
    uuid = 10;
    name = "Base Entity";
    start();
}

BaseEntity::~BaseEntity() {
    stop();
    components.clearAll();
}

void BaseEntity::start() {
}

void BaseEntity::update(float dt) {
    components.updateAll(dt);
}

void BaseEntity::stop() {
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
