#pragma once

#include <vector>
#include <string>

#include "../EntityState/EntityState.h"

class BaseComponent {
  public:
    BaseComponent();
    ~BaseComponent();
    virtual void start();
    virtual void update(float dt, EntityState& state);
    virtual void stop();

    int uuid;
    std::string name;
};
