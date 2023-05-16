#include "ComponentStorage.h"

void ComponentStorage::startAll(){
    start<BaseComponent>();
    start<TransformComponent>();
    start<ScriptComponent>();
    start<CameraComponent>();
    start<AudioSourceComponent>();
    start<ModelComponent>();
    start<SkeletalModelComponent>();
    start<LightComponent>();
    start<SkyBoxComponent>();
}
void ComponentStorage::updateAll(float dt, EntityState& state){
    update<BaseComponent>(dt,state);
    update<TransformComponent>(dt,state);
    update<ScriptComponent>(dt,state);
    update<CameraComponent>(dt,state);
    update<AudioSourceComponent>(dt,state);
    update<ModelComponent>(dt,state);
    update<SkeletalModelComponent>(dt,state);
    update<LightComponent>(dt,state);
    update<SkyBoxComponent>(dt,state);
}
void ComponentStorage::clearAll(){
    vecBaseComponent.clear();
    vecTransformComponent.clear();
    vecScriptComponent.clear();
    vecCameraComponent.clear();
    vecAudioSourceComponent.clear();
    vecModelComponent.clear();
    vecSkeletalModelComponent.clear();
    vecLightComponent.clear();
    vecSkyBoxComponent.clear();
}
void* ComponentStorage::getProtectedPtr(const ComponentLocation& loc){
    switch(loc.type){
    case ComponentLocation::BASECOMPONENT:
        if(loc.componentIdx>=vecBaseComponent.size())
            return nullptr;
        return &vecBaseComponent[loc.componentIdx];
    case ComponentLocation::TRANSFORMCOMPONENT:
        if(loc.componentIdx>=vecTransformComponent.size())
            return nullptr;
        return &vecTransformComponent[loc.componentIdx];
    case ComponentLocation::SCRIPTCOMPONENT:
        if(loc.componentIdx>=vecScriptComponent.size())
            return nullptr;
        return &vecScriptComponent[loc.componentIdx];
    case ComponentLocation::CAMERACOMPONENT:
        if(loc.componentIdx>=vecCameraComponent.size())
            return nullptr;
        return &vecCameraComponent[loc.componentIdx];
    case ComponentLocation::AUDIOSOURCECOMPONENT:
        if(loc.componentIdx>=vecAudioSourceComponent.size())
            return nullptr;
        return &vecAudioSourceComponent[loc.componentIdx];
    case ComponentLocation::MODELCOMPONENT:
        if(loc.componentIdx>=vecModelComponent.size())
            return nullptr;
        return &vecModelComponent[loc.componentIdx];
    case ComponentLocation::SKELETALMODELCOMPONENT:
        if(loc.componentIdx>=vecSkeletalModelComponent.size())
            return nullptr;
        return &vecSkeletalModelComponent[loc.componentIdx];
    case ComponentLocation::LIGHTCOMPONENT:
        if(loc.componentIdx>=vecLightComponent.size())
            return nullptr;
        return &vecLightComponent[loc.componentIdx];
    case ComponentLocation::SKYBOXCOMPONENT:
        if(loc.componentIdx>=vecSkyBoxComponent.size())
            return nullptr;
        return &vecSkyBoxComponent[loc.componentIdx];
    default:;
    }
    return nullptr;
}
void ComponentStorage::pushLuaTable(void* ptr,const ComponentLocation& loc,lua_State* state){
    switch(loc.type){
    case ComponentLocation::BASECOMPONENT:
        if(ptr==nullptr)
            break;
        ((BaseComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::TRANSFORMCOMPONENT:
        if(ptr==nullptr)
            break;
        ((TransformComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::SCRIPTCOMPONENT:
        if(ptr==nullptr)
            break;
        ((ScriptComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::CAMERACOMPONENT:
        if(ptr==nullptr)
            break;
        ((CameraComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::AUDIOSOURCECOMPONENT:
        if(ptr==nullptr)
            break;
        ((AudioSourceComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::MODELCOMPONENT:
        if(ptr==nullptr)
            break;
        ((ModelComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::SKELETALMODELCOMPONENT:
        if(ptr==nullptr)
            break;
        ((SkeletalModelComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::LIGHTCOMPONENT:
        if(ptr==nullptr)
            break;
        ((LightComponent*)ptr)->pushLuaTable(state);
        return;
    case ComponentLocation::SKYBOXCOMPONENT:
        if(ptr==nullptr)
            break;
        ((SkyBoxComponent*)ptr)->pushLuaTable(state);
        return;
    default:;
    }
    lua_pushnil(state);
    return;
}
