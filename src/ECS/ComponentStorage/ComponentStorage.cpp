#include "ComponentStorage.h"

void ComponentStorage::updateAll(float dt){
    update<BaseComponent>(dt);
    update<TransformComponent>(dt);
    update<ScriptComponent>(dt);
    update<ModelComponent>(dt);
}
void ComponentStorage::clearAll(){
    vecBaseComponent.clear();
    vecTransformComponent.clear();
    vecScriptComponent.clear();
    vecModelComponent.clear();
}