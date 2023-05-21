#pragma once

#include <string>

#include "BaseComponent.h"
#include "../../asset_import/model.h"
#include "../../asset_import/materials.h"
#include "../../model_import/model.h"
#include "../EntityState/EntityState.h"

struct ModelComponent : BaseComponent {
    ModelComponent();
    ModelComponent(const std::string& name, const int uuid);
    ~ModelComponent() {
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    void readMaterials();
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();
    static int genUuid();

    // model info
    std::string modelUuid = std::string("");
    std::shared_ptr<model::ModelDescriptor> modelDescriptor;

    std::vector<std::shared_ptr<ActiveMaterial>> materials;
};
