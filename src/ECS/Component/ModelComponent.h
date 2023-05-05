#pragma once

#include <string>

#include "BaseComponent.h"
#include "../../asset_import/model.h"
#include "../../model_import/model.h"
#include "../EntityState/EntityState.h"

struct ModelComponent : BaseComponent {
    ModelComponent();
    ~ModelComponent() {
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    // model info
    std::string modelUuid = std::string("");
    std::shared_ptr<model::ModelDescriptor> modelDescriptor;

    // no controls for now
};
