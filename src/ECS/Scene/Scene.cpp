#include "Scene.h"

#include <glm/mat4x4.hpp>

Scene::Scene() {
    selectedEntity = nullptr;
    start();
}

Scene::~Scene() {
    stop();
}

void Scene::start() {
}

void Scene::updatePositions() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities[i].genTransform(entities[i].parent < 0
                ? glm::mat4(1.0f)
                : entities[entities[i].parent].runtimeTransform);
    }
}

void Scene::update(float dt) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities[i].update(dt);
    }
}

void Scene::stop() {
}

void Scene::addEntity(const BaseEntity& entity) {
    // generate uuid
    // add uuid to entity
    entities.emplace_back(entity);
}

void Scene::addChild(const BaseEntity& entity, int parentIdx) {
    addEntity(entity);
    setParent(entities.size() - 1, parentIdx);
}

// void Scene::removeEntity(int uuid) {
//     for (unsigned int i = 0; i < entities.size(); i++) {
//         if (entities[i].uuid == uuid) {
//             entities.erase(entities.begin() + i);
//         }
//     }
// }

void Scene::selectEntity(BaseEntity* entity) {
    selectedEntity = entity;
}

void Scene::setParent(int childIdx, int parentIdx) {
    // edit parent index
    entities[childIdx].parent = parentIdx;

    // reorganize array
    // TODO linked list struct?
    // swap right if needed
    int newIdx = childIdx;
    for (; newIdx < parentIdx; newIdx++) {
        // TODO children of child
        // correct parent if needed
        if (entities[newIdx + 1].parent > childIdx)
            entities[newIdx + 1].parent--;
        std::swap(entities[newIdx], entities[newIdx + 1]);
    }

    // swap left if needed
    for (; newIdx > parentIdx + 1; newIdx--) {
        // TODO children of child
        // correct parent if needed
        if (entities[newIdx - 1].parent > parentIdx)
            entities[newIdx - 1].parent++;
        std::swap(entities[newIdx], entities[newIdx - 1]);
    }
}
