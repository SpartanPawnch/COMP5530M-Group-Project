#pragma once

#include <string>

#include <glm/vec3.hpp>

#include "BaseComponent.h"
#include "../../asset_import/audio.h"

struct AudioSourceComponent : BaseComponent {
    AudioSourceComponent() {
        name = "Audio Source Component";
    }
    ~AudioSourceComponent() {
    }
    void start() override;
    void update(float dt) override;
    void stop() override;
    void startSound();

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
