#include "ComponentStorage.h"

void ComponentStorage::startAll(){
    start<BaseComponent>();
    start<TransformComponent>();
    start<ScriptComponent>();
    start<CameraComponent>();
    start<AudioSourceComponent>();
    start<ModelComponent>();
    start<SkeletalModelComponent>();
}
void ComponentStorage::updateAll(float dt){
    update<BaseComponent>(dt);
    update<TransformComponent>(dt);
    update<ScriptComponent>(dt);
    update<CameraComponent>(dt);
    update<AudioSourceComponent>(dt);
    update<ModelComponent>(dt);
    update<SkeletalModelComponent>(dt);
}
void ComponentStorage::clearAll(){
    vecBaseComponent.clear();
    vecTransformComponent.clear();
    vecScriptComponent.clear();
    vecCameraComponent.clear();
    vecAudioSourceComponent.clear();
    vecModelComponent.clear();
    vecSkeletalModelComponent.clear();
}
