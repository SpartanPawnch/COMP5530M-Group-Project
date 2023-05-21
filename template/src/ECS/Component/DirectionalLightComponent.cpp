#include "DirectionalLightComponent.h"

#include <glm/vec4.hpp>

#include "../../../render-engine/LightSource.h"
#include "../../../render-engine/RenderManager.h"
#include "../../scripting.h"

static int baseUuid = 0;

static RenderManager* renderMgr;

int DirectionalLightComponent::genUuid() {
    return baseUuid++;
}

DirectionalLightComponent::DirectionalLightComponent() {
    uuid = baseUuid++;
    name = "Directional Light Component";
    renderMgr = RenderManager::getInstance();
    desc = renderMgr->addDirectionalLightSource(direction, ambient, diffuse, specular);
}

DirectionalLightComponent::DirectionalLightComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
    renderMgr = RenderManager::getInstance();
    desc = renderMgr->addDirectionalLightSource(direction, ambient, diffuse, specular);
}

DirectionalLightComponent::~DirectionalLightComponent() {
    if (desc.use_count() == 1)
        renderMgr->removeDirectionalLightSource(desc->idx);
}
void DirectionalLightComponent::start() {
}
void DirectionalLightComponent::update(float dt, EntityState& state) {
    LightSource* lightPtr = renderMgr->getDirectionalLightSource(desc->idx);
    lightPtr->setPosition(glm::vec4(direction, 1.f));
    lightPtr->setAmbient(ambient);
    lightPtr->setDiffuse(diffuse);
    lightPtr->setSpecular(specular);
}
void DirectionalLightComponent::stop() {
}

// lua stuff
static const char* componentMT = "ONO_DirectionalLightComponent";
void DirectionalLightComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pop(state, 1);
}

void DirectionalLightComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    scripting::pushVec3Ref(state, &ambient);
    lua_setfield(state, -2, "ambient");
    scripting::pushVec3Ref(state, &diffuse);
    lua_setfield(state, -2, "diffuse");
    scripting::pushVec3Ref(state, &specular);
    lua_setfield(state, -2, "specular");
    scripting::pushVec3Ref(state, &direction);
    lua_setfield(state, -2, "position");
    luaL_setmetatable(state, componentMT);
}
