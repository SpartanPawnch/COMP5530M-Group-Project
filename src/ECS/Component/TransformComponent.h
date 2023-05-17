#pragma once

#include <vector>
#include "glm/glm.hpp"
#include <glm/gtc/quaternion.hpp>

#include "BaseComponent.h"
#include "../EntityState/EntityState.h"

class TransformComponent : public BaseComponent {
  public:
    TransformComponent();
    TransformComponent(const std::string& name, const int uuid);
    ~TransformComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();

    int uuid;
    glm::vec3 position = glm::vec3(.0f);
    glm::quat rotation = glm::quat(1.0f, glm::vec3(.0f));
    glm::vec3 scale = glm::vec3(1.0f);
};
