#include "Scene.h"

#include <glm/mat4x4.hpp>

Scene::Scene() {
    selectedEntity = nullptr;
}

Scene::~Scene() {
    stop();
}

void Scene::start() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities[i].start();
    }
}

void Scene::updatePositions() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        entities[i].genTransform(entities[i].parent < 0
                ? glm::mat4(1.0f)
                : entities[entities[i].parent].state.runtimeTransform);
    }
}

void Scene::update(float dt) {
    for (unsigned int i = 0; i < entities.size(); i++) {
        glm::mat4 parentTransform = entities[i].parent < 0
            ? glm::mat4(1.0f)
            : entities[entities[i].parent].state.runtimeTransform;
        entities[i].update(parentTransform, dt);
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

void Scene::removeEntityByIdx(int idx) {
    // move to back
    int parentIdx = entities[idx].parent;
    for (int i = idx; i < entities.size() - 1; i++) {
        if (entities[i + 1].parent == idx)
            entities[i + 1].parent = parentIdx;

        else if (entities[i + 1].parent > idx)
            entities[i + 1].parent--;

        std::swap(entities[i], entities[i + 1]);
    }

    // delete
    entities.pop_back();
}

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
    if (newIdx < parentIdx || parentIdx < 0) {
        for (; newIdx < parentIdx || (parentIdx < 0 && newIdx < entities.size() - 1); newIdx++) {
            // correct parent if needed
            if (entities[newIdx + 1].parent > childIdx)
                entities[newIdx + 1].parent--;
            std::swap(entities[newIdx], entities[newIdx + 1]);
        }
        entities[newIdx].parent = parentIdx >= 0 ? newIdx - 1 : -1;

        // handle children
        while (entities[childIdx].parent == childIdx) {
            setParent(childIdx, newIdx);
            newIdx--;
        }
    }
    else {
        // swap left if needed
        for (; newIdx > parentIdx + 1; newIdx--) {
            // correct parent if needed
            if (entities[newIdx - 1].parent > parentIdx)
                entities[newIdx - 1].parent++;
            std::swap(entities[newIdx], entities[newIdx - 1]);
        }

        // handle children
        for (int i = childIdx + 1; i < entities.size() && entities[i].parent == childIdx; i++) {
            setParent(i, newIdx);
        }
    }
}
