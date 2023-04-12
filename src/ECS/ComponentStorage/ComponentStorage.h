#pragma once

#include <vector>

#include "../Component/BaseComponent.h"
#include "../Component/TransformComponent.h"
#include "../Component/ScriptComponent.h"

struct ComponentStorage{
    std::vector<BaseComponent> vecBaseComponent;
    std::vector<TransformComponent> vecTransformComponent;
    std::vector<ScriptComponent> vecScriptComponent;
    
    //add component, type is inferred by compiler
    template<typename T>
    void addComponent(T&& component);

    //update all components of specific type
    template<typename T>
    void update(float dt);

    //calls update for all types
    void updateAll(float dt);
    
    //clear all components
    void clearAll();
};
