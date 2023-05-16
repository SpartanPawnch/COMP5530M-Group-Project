#pragma once

#include <vector>

#include "BaseEntity.h"
#include "../Component/BaseComponent.h"
#include "../Component/TransformComponent.h"

class CameraEntity : public BaseEntity {
  public:
    CameraEntity();
    ~CameraEntity();
    virtual void start() override;
    virtual void stop() override;
};
