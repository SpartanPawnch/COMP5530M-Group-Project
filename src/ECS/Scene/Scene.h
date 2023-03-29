#pragma once

#include <vector>

#include "../Entity/BaseEntity.h"
#include "../Component/BaseComponent.h"

class Scene{
public:
    Scene();
    ~Scene();
    void start();
    void update(float dt);
    void stop();
    void addEntity(BaseEntity entity);
    void removeEntity(int uuid);

    int selectedEntity;
    std::vector<BaseEntity> entities;
};