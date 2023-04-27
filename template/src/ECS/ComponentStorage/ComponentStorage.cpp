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
void ComponentStorage::updateAll(float dt, EntityState& state){
    update<BaseComponent>(dt,state);
    update<TransformComponent>(dt,state);
    update<ScriptComponent>(dt,state);
    update<CameraComponent>(dt,state);
    update<AudioSourceComponent>(dt,state);
    update<ModelComponent>(dt,state);
    update<SkeletalModelComponent>(dt,state);
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
