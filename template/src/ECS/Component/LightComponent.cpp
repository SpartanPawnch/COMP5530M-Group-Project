#include "LightComponent.h"

#include <glm/vec4.hpp>

#include "../../../render-engine/LightSource.h"
#include "../../../render-engine/RenderManager.h"
#include "../../scripting.h"

static int baseUuid = 0;

static RenderManager* renderMgr;

LightComponent::LightComponent() {
    uuid = baseUuid++;
    name = "Point Light Component";
    renderMgr = RenderManager::getInstance();
    desc = renderMgr->addLightSource(position, ambient, diffuse, specular);
}

LightComponent::LightComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
    renderMgr = RenderManager::getInstance();
    desc = renderMgr->addLightSource(position, ambient, diffuse, specular);
}

LightComponent::~LightComponent() {
    if (desc.use_count() == 1)
        renderMgr->removeLightSource(desc->idx);
}
void LightComponent::start() {
}
void LightComponent::update(float dt, EntityState& state) {
    LightSource* lightPtr = renderMgr->getLightSource(desc->idx);
    lightPtr->setPosition(glm::vec3(state.runtimeTransform * glm::vec4(position, 1.f)));
    lightPtr->setAmbient(ambient);
    lightPtr->setDiffuse(diffuse);
    lightPtr->setSpecular(specular);
}
void LightComponent::stop() {
}

// lua stuff
static const char* componentMT = "ONO_LightComponent";
void LightComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pop(state, 1);
}

void LightComponent::pushLuaTable(lua_State* state) {
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
    scripting::pushVec3Ref(state, &position);
    lua_setfield(state, -2, "position");
    luaL_setmetatable(state, componentMT);
}
