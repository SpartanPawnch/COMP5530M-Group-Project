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

void Scene::updateReferences() {
    for (unsigned int i = 0; i < entities.size(); i++) {
        std::vector<ScriptComponent>& scripts = entities[i].components.vecScriptComponent;
        for (unsigned int j = 0; j < scripts.size(); j++) {
            for (unsigned int k = 0; k < scripts[j].args.size(); k++) {
                // update entity pointer
                if (scripts[j].args[k].type == ScriptArgument::ENTITY) {
                    int idx = uuidToIdx.count(scripts[j].args[k].arg._int) > 0
                        ? uuidToIdx[scripts[j].args[k].arg._int]
                        : -1;
                    scripts[j].args[k].ref =
                        (idx >= 0 && idx < entities.size()) ? &entities[idx].state : nullptr;
                }
                // update component pointer
                else if (scripts[j].args[k].type == ScriptArgument::COMPONENT) {
                    ComponentLocation& loc = scripts[j].args[k].arg._loc;
                    // handle invalid entity
                    if (uuidToIdx.count(loc.entityUuid) == 0) {
                        scripts[j].args[k].ref = nullptr;
                        continue;
                    }
                    scripts[j].args[k].ref =
                        entities[uuidToIdx[loc.entityUuid]].components.getProtectedPtr(loc);
                }
            }
        }
    }
}

void Scene::fixDescriptors(int entityUuid, ComponentLocation::CompType deletedType,
    int deletedIdx) {
    for (size_t i = 0; i < entities.size(); i++) {
        std::vector<ScriptComponent>& scripts = entities[i].components.vecScriptComponent;
        for (size_t j = 0; j < scripts.size(); j++) {
            for (unsigned int k = 0; k < scripts[j].args.size(); k++) {
                if (scripts[j].args[k].type == ScriptArgument::COMPONENT &&
                    scripts[j].args[k].arg._loc.type == deletedType &&
                    scripts[j].args[k].arg._loc.entityUuid == entityUuid) {
                    if (scripts[j].args[k].arg._loc.componentIdx > deletedIdx) {
                        scripts[j].args[k].arg._loc.componentIdx--;
                    }
                    else if (scripts[j].args[k].arg._loc.componentIdx == deletedIdx) {
                        scripts[j].args[k].arg._loc.componentIdx = -1;
                    }
                }
            }
        }
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
    // add uuid to entity
    entities.emplace_back(entity);
    uuidToIdx[entity.uuid] = entities.size() - 1;
}

void Scene::addChild(const BaseEntity& entity, int parentIdx) {
    addEntity(entity);
    setParent(entities.size() - 1, parentIdx);
}

void Scene::removeEntityByIdx(int idx) {
    // remove from map
    uuidToIdx.erase(entities[idx].uuid);

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
    // swap right if needed
    int newIdx = childIdx;
    if (newIdx < parentIdx || parentIdx < 0) {
        uuidToIdx[entities[childIdx].uuid] = parentIdx < 0 ? entities.size() - 1 : parentIdx;
        for (; newIdx < parentIdx || (parentIdx < 0 && newIdx < entities.size() - 1); newIdx++) {
            // correct parent if needed
            if (entities[newIdx + 1].parent > childIdx)
                entities[newIdx + 1].parent--;
            // fix uuidToIdx
            uuidToIdx[entities[newIdx + 1].uuid]--;
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
        uuidToIdx[entities[childIdx].uuid] = parentIdx + 1;
        // swap left if needed
        for (; newIdx > parentIdx + 1; newIdx--) {
            // correct parent if needed
            if (entities[newIdx - 1].parent > parentIdx)
                entities[newIdx - 1].parent++;
            uuidToIdx[entities[newIdx - 1].uuid]++;
            std::swap(entities[newIdx], entities[newIdx - 1]);
        }

        // handle children
        for (int i = childIdx + 1; i < entities.size() && entities[i].parent == childIdx; i++) {
            setParent(i, newIdx);
        }
    }
}
