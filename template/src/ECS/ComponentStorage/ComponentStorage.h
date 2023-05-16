#pragma once

#include <vector>

#include "../Component/BaseComponent.h"
#include "../Component/TransformComponent.h"
#include "../Component/ScriptComponent.h"
#include "../Component/AudioSourceComponent.h"

struct ComponentStorage{
    std::vector<BaseComponent> vecBaseComponent;
    std::vector<TransformComponent> vecTransformComponent;
    std::vector<ScriptComponent> vecScriptComponent;
    std::vector<AudioSourceComponent> vecAudioSourceComponent;
    //add component, type is inferred by compiler
    template<typename T>
    void addComponent(const T& component);

    //start all components of specific type
    template<typename T>
    void start();

    //update all components of specific type
    template<typename T>
    void update(float dt);

    //call start for all types
    void startAll();

    //call update for all types
    void updateAll(float dt);
    
    //clear all components
    void clearAll();

    // --- Template Specializations ---
    template<>
    void addComponent<BaseComponent>(const BaseComponent& component){
        vecBaseComponent.emplace_back(component);
    }

    template<>
    void addComponent<TransformComponent>(const TransformComponent& component){
        vecTransformComponent.emplace_back(component);
    }

    template<>
    void addComponent<ScriptComponent>(const ScriptComponent& component){
        vecScriptComponent.emplace_back(component);
    }

    template<>
    void addComponent<AudioSourceComponent>(const AudioSourceComponent& component){
        vecAudioSourceComponent.emplace_back(component);
    }

    template<>
    void start<BaseComponent>(){
        for(unsigned int i=0;i<vecBaseComponent.size();i++){
            vecBaseComponent[i].start();
        }
    }

    template<>
    void start<TransformComponent>(){
        for(unsigned int i=0;i<vecTransformComponent.size();i++){
            vecTransformComponent[i].start();
        }
    }

    template<>
    void start<ScriptComponent>(){
        for(unsigned int i=0;i<vecScriptComponent.size();i++){
            vecScriptComponent[i].start();
        }
    }

    template<>
    void start<AudioSourceComponent>(){
        for(unsigned int i=0;i<vecAudioSourceComponent.size();i++){
            vecAudioSourceComponent[i].start();
        }
    }

    template<>
    void update<BaseComponent>(float dt){
        for(unsigned int i=0;i<vecBaseComponent.size();i++){
            vecBaseComponent[i].update(dt);
        }
    }

    template<>
    void update<TransformComponent>(float dt){
        for(unsigned int i=0;i<vecTransformComponent.size();i++){
            vecTransformComponent[i].update(dt);
        }
    }

    template<>
    void update<ScriptComponent>(float dt){
        for(unsigned int i=0;i<vecScriptComponent.size();i++){
            vecScriptComponent[i].update(dt);
        }
    }

    template<>
    void update<AudioSourceComponent>(float dt){
        for(unsigned int i=0;i<vecAudioSourceComponent.size();i++){
            vecAudioSourceComponent[i].update(dt);
        }
    }

};