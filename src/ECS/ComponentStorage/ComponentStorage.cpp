#include "ComponentStorage.h"

template<>
void ComponentStorage::addComponent<BaseComponent>(BaseComponent&& component){
    vecBaseComponent.emplace_back(component);
}

template<>
void ComponentStorage::addComponent<TransformComponent>(TransformComponent&& component){
    vecTransformComponent.emplace_back(component);
}

template<>
void ComponentStorage::addComponent<ScriptComponent>(ScriptComponent&& component){
    vecScriptComponent.emplace_back(component);
}

template<>
void ComponentStorage::update<BaseComponent>(float dt){
    for(unsigned int i=0;i<vecBaseComponent.size();i++){
        vecBaseComponent[i].update(dt);
    }
}

template<>
void ComponentStorage::update<TransformComponent>(float dt){
    for(unsigned int i=0;i<vecTransformComponent.size();i++){
        vecTransformComponent[i].update(dt);
    }
}

template<>
void ComponentStorage::update<ScriptComponent>(float dt){
    for(unsigned int i=0;i<vecScriptComponent.size();i++){
        vecScriptComponent[i].update(dt);
    }
}

void ComponentStorage::updateAll(float dt){
    update<BaseComponent>(dt);
    update<TransformComponent>(dt);
    update<ScriptComponent>(dt);
}
void ComponentStorage::clearAll(){
    vecBaseComponent.clear();
    vecTransformComponent.clear();
    vecScriptComponent.clear();
}
