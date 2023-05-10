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
    // void removeComponent(int uuid);

    EntityState state;

    // look for the best uuid generator
    int uuid;
    std::string name;

    int parent = -1;

    ComponentStorage components;
};
