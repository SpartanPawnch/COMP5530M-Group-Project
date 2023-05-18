#pragma once

#include <vector>
#include <unordered_map>

#include "../Entity/BaseEntity.h"
#include "../Component/BaseComponent.h"

class Scene {
  public:
    Scene();
    ~Scene();
    void start();
    void updatePositions();
    void updateReferences();
    void update(float dt);
    void stop();
    void addEntity(const BaseEntity& entity);
    void addChild(const BaseEntity& entity, int parentIdx);
    void removeEntityByIdx(int idx);
    void selectEntity(BaseEntity* entity);
    void setParent(int childIdx, int parentIdx);
    void fixDescriptors(int entityUuid, ComponentLocation::CompType deletedType, int deletedIdx);
    void registerLuaTable();

    BaseEntity* selectedEntity;
    int selectedCameraIdx = 0;

#ifdef ONO_ENGINE_ONLY
    bool showColliders = false;
#endif

    std::unordered_map<int, int> uuidToIdx;
    std::vector<BaseEntity> entities;
};
