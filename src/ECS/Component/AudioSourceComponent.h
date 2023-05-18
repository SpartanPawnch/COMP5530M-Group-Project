#pragma once

#include <string>

#include <glm/vec3.hpp>

#include "BaseComponent.h"
#include "../../asset_import/audio.h"
#include "../EntityState/EntityState.h"

struct AudioSourceComponent : BaseComponent {
    AudioSourceComponent();
    AudioSourceComponent(const std::string& name, const int uuid);
    ~AudioSourceComponent() {
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;
    void startSound();
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();

    // internal

    // audio info
    std::string clipUuid = std::string("");
    std::shared_ptr<audio::AudioDescriptor> clipDescriptor;

    // controls
    bool loop = false;
    bool directional = false;
    bool playOnStart = true;
    glm::vec3 position = glm::vec3(.0f);
};
