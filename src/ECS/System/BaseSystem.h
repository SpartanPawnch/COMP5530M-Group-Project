#pragma once

#include <vector>

#include "../Entity/BaseEntity.h"
#include "../Component/BaseComponent.h"

class BaseSystem {
  public:
    BaseSystem() {
    }
    ~BaseSystem() {
    }
    virtual void start() {
    }
    virtual void update(float dt) {
    }
    virtual void stop() {
    }
};
