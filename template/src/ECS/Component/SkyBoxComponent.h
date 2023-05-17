#pragma once

#include <string>
#include <utility>
#include "BaseComponent.h"
#include "../../asset_import/images.h"
#include "../../asset_import/folders.h"

struct SkyBoxFace {
    std::string path = std::string("");
    std::shared_ptr<TextureDescriptor> textureDescriptor;
    std::string textureUuid = std::string("");
};

struct SkyBoxTexture {
    unsigned int id = 0;
    SkyBoxFace faces[6];
};

struct SkyBoxComponent : BaseComponent {
    SkyBoxComponent();
    SkyBoxComponent(const std::string& name, const int uuid);
    ~SkyBoxComponent() {
    }
    void start() override;
    void update(float dt, EntityState& state) override;
    void stop() override;

    void updateTex(unsigned int side, std::string path);
    void pushLuaTable(lua_State* state);
    static void registerLuaMetatable();

    SkyBoxTexture skybox;
};
