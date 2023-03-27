#pragma once

#include <vector>
#include "Scene/Scene.h"

#include "Entity/BaseEntity.h"
#include "Entity/ModelEntity.h"
#include "Entity/CameraEntity.h"

#include "Component/BaseComponent.h"

#include "System/BaseSystem.h"

class Engine{
public:
    Engine();
    ~Engine();
    void start();
    void update();
    void stop();
    void openScene(int scene);

    int shouldClose;
    int currentScene;
    std::vector<Scene> scenes;
    std::vector<BaseSystem> systems;
};