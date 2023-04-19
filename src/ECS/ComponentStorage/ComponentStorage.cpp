#include "ComponentStorage.h"

void ComponentStorage::startAll(){
    start<BaseComponent>();
    start<TransformComponent>();
    start<ScriptComponent>();
    start<AudioSourceComponent>();
}
void ComponentStorage::updateAll(float dt){
    update<BaseComponent>(dt);
    update<TransformComponent>(dt);
    update<ScriptComponent>(dt);
    update<AudioSourceComponent>(dt);
}
void ComponentStorage::clearAll(){
    vecBaseComponent.clear();
    vecTransformComponent.clear();
    vecScriptComponent.clear();
    vecAudioSourceComponent.clear();
}
