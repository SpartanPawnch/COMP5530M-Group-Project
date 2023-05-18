#pragma once

#include <vector>

#include <lua.hpp>

#include "../EntityState/EntityState.h"
#include "../ComponentLocation/ComponentLocation.h"

#include "../Component/BaseComponent.h"
#include "../Component/PlayerControllerComponent.h"
#include "../Component/ScriptComponent.h"
#include "../Component/CameraComponent.h"
#include "../Component/AudioSourceComponent.h"
#include "../Component/ModelComponent.h"
#include "../Component/SkeletalModelComponent.h"
#include "../Component/LightComponent.h"
#include "../Component/SkyBoxComponent.h"
#include "../Component/RigidBodyComponent.h"
#include "../Component/ControllerComponent.h"
#include "../Component/TransformComponent.h"

struct ComponentStorage{
    std::vector<BaseComponent> vecBaseComponent;
    std::vector<PlayerControllerComponent> vecPlayerControllerComponent;
    std::vector<ScriptComponent> vecScriptComponent;
    std::vector<CameraComponent> vecCameraComponent;
    std::vector<AudioSourceComponent> vecAudioSourceComponent;
    std::vector<ModelComponent> vecModelComponent;
    std::vector<SkeletalModelComponent> vecSkeletalModelComponent;
    std::vector<LightComponent> vecLightComponent;
    std::vector<SkyBoxComponent> vecSkyBoxComponent;
    std::vector<RigidBodyComponent> vecRigidBodyComponent;
    std::vector<ControllerComponent> vecControllerComponent;
    std::vector<TransformComponent> vecTransformComponent;
    //add component, type is inferred by compiler
    template<typename T>
    void addComponent(const T& component);

    //start all components of specific type
    template<typename T>
    void start();

    //update all components of specific type
    template<typename T>
    void update(float dt,EntityState& state);

    //call start for all types
    void startAll();

    //call update for all types
    void updateAll(float dt,EntityState& state);
    
    //clear all components
    void clearAll();

    //get raw pointer using component loc
    void* getProtectedPtr(const ComponentLocation& loc);

    //register metatables
    static void registerMetatables();

    //push lua table
    static void pushLuaTable(void* ptr, const ComponentLocation& loc, lua_State* state);

    //get CompType enum based on type
    template<typename T>
    static ComponentLocation::CompType typeToCompTypeEnum();

    // --- Template Specializations ---
    template<>
    void addComponent<BaseComponent>(const BaseComponent& component){
        vecBaseComponent.emplace_back(component);
    }

    template<>
    void addComponent<PlayerControllerComponent>(const PlayerControllerComponent& component){
        vecPlayerControllerComponent.emplace_back(component);
    }

    template<>
    void addComponent<ScriptComponent>(const ScriptComponent& component){
        vecScriptComponent.emplace_back(component);
    }

    template<>
    void addComponent<CameraComponent>(const CameraComponent& component){
        vecCameraComponent.emplace_back(component);
    }

    template<>
    void addComponent<AudioSourceComponent>(const AudioSourceComponent& component){
        vecAudioSourceComponent.emplace_back(component);
    }

    template<>
    void addComponent<ModelComponent>(const ModelComponent& component){
        vecModelComponent.emplace_back(component);
    }

    template<>
    void addComponent<SkeletalModelComponent>(const SkeletalModelComponent& component){
        vecSkeletalModelComponent.emplace_back(component);
    }

    template<>
    void addComponent<LightComponent>(const LightComponent& component){
        vecLightComponent.emplace_back(component);
    }

    template<>
    void addComponent<SkyBoxComponent>(const SkyBoxComponent& component){
        vecSkyBoxComponent.emplace_back(component);
    }

    template<>
    void addComponent<RigidBodyComponent>(const RigidBodyComponent& component){
        vecRigidBodyComponent.emplace_back(component);
    }

    template<>
    void addComponent<ControllerComponent>(const ControllerComponent& component){
        vecControllerComponent.emplace_back(component);
    }

    template<>
    void addComponent<TransformComponent>(const TransformComponent& component){
        vecTransformComponent.emplace_back(component);
    }

    template<>
    void start<BaseComponent>(){
        for(unsigned int i=0;i<vecBaseComponent.size();i++){
            vecBaseComponent[i].start();
        }
    }

    template<>
    void start<PlayerControllerComponent>(){
        for(unsigned int i=0;i<vecPlayerControllerComponent.size();i++){
            vecPlayerControllerComponent[i].start();
        }
    }

    template<>
    void start<ScriptComponent>(){
        for(unsigned int i=0;i<vecScriptComponent.size();i++){
            vecScriptComponent[i].start();
        }
    }

    template<>
    void start<CameraComponent>(){
        for(unsigned int i=0;i<vecCameraComponent.size();i++){
            vecCameraComponent[i].start();
        }
    }

    template<>
    void start<AudioSourceComponent>(){
        for(unsigned int i=0;i<vecAudioSourceComponent.size();i++){
            vecAudioSourceComponent[i].start();
        }
    }

    template<>
    void start<ModelComponent>(){
        for(unsigned int i=0;i<vecModelComponent.size();i++){
            vecModelComponent[i].start();
        }
    }

    template<>
    void start<SkeletalModelComponent>(){
        for(unsigned int i=0;i<vecSkeletalModelComponent.size();i++){
            vecSkeletalModelComponent[i].start();
        }
    }

    template<>
    void start<LightComponent>(){
        for(unsigned int i=0;i<vecLightComponent.size();i++){
            vecLightComponent[i].start();
        }
    }

    template<>
    void start<SkyBoxComponent>(){
        for(unsigned int i=0;i<vecSkyBoxComponent.size();i++){
            vecSkyBoxComponent[i].start();
        }
    }

    template<>
    void start<RigidBodyComponent>(){
        for(unsigned int i=0;i<vecRigidBodyComponent.size();i++){
            vecRigidBodyComponent[i].start();
        }
    }

    template<>
    void start<ControllerComponent>(){
        for(unsigned int i=0;i<vecControllerComponent.size();i++){
            vecControllerComponent[i].start();
        }
    }

    template<>
    void start<TransformComponent>(){
        for(unsigned int i=0;i<vecTransformComponent.size();i++){
            vecTransformComponent[i].start();
        }
    }

    template<>
    void update<BaseComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecBaseComponent.size();i++){
            vecBaseComponent[i].update(dt,state);
        }
    }

    template<>
    void update<PlayerControllerComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecPlayerControllerComponent.size();i++){
            vecPlayerControllerComponent[i].update(dt,state);
        }
    }

    template<>
    void update<ScriptComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecScriptComponent.size();i++){
            vecScriptComponent[i].update(dt,state);
        }
    }

    template<>
    void update<CameraComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecCameraComponent.size();i++){
            vecCameraComponent[i].update(dt,state);
        }
    }

    template<>
    void update<AudioSourceComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecAudioSourceComponent.size();i++){
            vecAudioSourceComponent[i].update(dt,state);
        }
    }

    template<>
    void update<ModelComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecModelComponent.size();i++){
            vecModelComponent[i].update(dt,state);
        }
    }

    template<>
    void update<SkeletalModelComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecSkeletalModelComponent.size();i++){
            vecSkeletalModelComponent[i].update(dt,state);
        }
    }

    template<>
    void update<LightComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecLightComponent.size();i++){
            vecLightComponent[i].update(dt,state);
        }
    }

    template<>
    void update<SkyBoxComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecSkyBoxComponent.size();i++){
            vecSkyBoxComponent[i].update(dt,state);
        }
    }

    template<>
    void update<RigidBodyComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecRigidBodyComponent.size();i++){
            vecRigidBodyComponent[i].update(dt,state);
        }
    }

    template<>
    void update<ControllerComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecControllerComponent.size();i++){
            vecControllerComponent[i].update(dt,state);
        }
    }

    template<>
    void update<TransformComponent>(float dt,EntityState& state){
        for(unsigned int i=0;i<vecTransformComponent.size();i++){
            vecTransformComponent[i].update(dt,state);
        }
    }

    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<BaseComponent>(){
        return ComponentLocation::BASECOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<PlayerControllerComponent>(){
        return ComponentLocation::PLAYERCONTROLLERCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<ScriptComponent>(){
        return ComponentLocation::SCRIPTCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<CameraComponent>(){
        return ComponentLocation::CAMERACOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<AudioSourceComponent>(){
        return ComponentLocation::AUDIOSOURCECOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<ModelComponent>(){
        return ComponentLocation::MODELCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<SkeletalModelComponent>(){
        return ComponentLocation::SKELETALMODELCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<LightComponent>(){
        return ComponentLocation::LIGHTCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<SkyBoxComponent>(){
        return ComponentLocation::SKYBOXCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<RigidBodyComponent>(){
        return ComponentLocation::RIGIDBODYCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<ControllerComponent>(){
        return ComponentLocation::CONTROLLERCOMPONENT;
    }
    template<>
    static ComponentLocation::CompType typeToCompTypeEnum<TransformComponent>(){
        return ComponentLocation::TRANSFORMCOMPONENT;
    }
};