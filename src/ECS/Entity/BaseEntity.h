#pragma once

#include <vector>
#include <string>

#include "../Component/BaseComponent.h"
#include "../ComponentStorage/ComponentStorage.h"

// Requires Change!!
class BaseEntity {
  public:
    BaseEntity();
    ~BaseEntity();
    virtual void start();
    virtual void update(float dt);
    virtual void stop();
    // void removeComponent(int uuid);

    // look for the best uuid generator
    int uuid;
    std::string name;
    ComponentStorage components;
};
