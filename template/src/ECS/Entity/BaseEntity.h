#pragma once

#include <vector>
#include <string>

#include "../Component/BaseComponent.h"
#include "../ComponentStorage/ComponentStorage.h"
#include "../EntityState/EntityState.h"

// Requires Change!!
class BaseEntity {
  public:
    BaseEntity();
    BaseEntity(const std::string& name, const int uuid);
    ~BaseEntity();
    virtual void start();
    void genTransform(const glm::mat4& parentMat);
    virtual void update(const glm::mat4& parentMat, float dt);
    virtual void stop();
    static int genUuid();

    EntityState state;

    int uuid = -1;
    bool active = true;
    bool visible = true;
    std::string name;

    int parent = -1;

    ComponentStorage components;
};
