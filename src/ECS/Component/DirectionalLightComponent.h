#pragma once
#include "BaseComponent.h"
#include <memory>
#include "../../../render-engine/LightSource.h"
struct DirectionalLightComponent : BaseComponent {
    std::shared_ptr<LightDescriptor> desc;
    glm::vec3 direction = glm::vec3(.0f);
    glm::vec3 ambient = glm::vec3(.2f);
    glm::vec3 diffuse = glm::vec3(.8f);
    glm::vec3 specular = glm::vec3(1.f);

    DirectionalLightComponent();
    DirectionalLightComponent(const std::string& name, const int uuid);
    ~DirectionalLightComponent();
    virtual void start() override;
    virtual void update(float dt, EntityState& state) override;
    virtual void stop() override;
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();
};
