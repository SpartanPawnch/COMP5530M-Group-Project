#pragma once

#include <vector>

#include "../Entity/BaseEntity.h"
#include "../Component/BaseComponent.h"

class Scene {
  public:
    Scene();
    ~Scene();
    void start();
    void updatePositions();
    void update(float dt);
    void stop();
    void addEntity(const BaseEntity& entity);
    void addChild(const BaseEntity& entity, int parentIdx);
    void removeEntityByIdx(int idx);
    void selectEntity(BaseEntity* entity);
    void setParent(int childIdx, int parentIdx);

    BaseEntity* selectedEntity;
    std::vector<BaseEntity> entities;
};