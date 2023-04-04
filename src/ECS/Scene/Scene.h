#pragma once

#include <vector>

#include "../Entity/BaseEntity.h"
#include "../Component/BaseComponent.h"

class Scene {
public:
    Scene();
    ~Scene();
    void start();
    void update(float dt);
    void stop();
    void addEntity(const BaseEntity& entity);
    void removeEntity(int uuid);
    void selectEntity(BaseEntity* entity);

    BaseEntity* selectedEntity;
    std::vector<BaseEntity> entities;
};