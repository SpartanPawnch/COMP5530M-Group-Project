#include "CameraComponent.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "../../../render-engine/Camera.h"
#include "../../../render-engine/RenderManager.h"
#include "../../scripting.h"

static int baseUuid = 0;

int CameraComponent::activeUuid = -1;

static RenderManager* renderMgr;

CameraComponent::CameraComponent() {
    name = "Camera Component";
    uuid = baseUuid++;
    renderMgr = RenderManager::getInstance();
    activeUuid = (activeUuid < 0) ? uuid : activeUuid;
}

CameraComponent::CameraComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    renderMgr = RenderManager::getInstance();
    baseUuid = std::max(baseUuid, _uuid + 1);
    activeUuid = (activeUuid < 0) ? uuid : activeUuid;
}

CameraComponent::~CameraComponent() {
}

void CameraComponent::start() {
}

glm::mat4 CameraComponent::getMatrix() {
    return glm::lookAt(eye, center, up);
}

void CameraComponent::copyToCamera(Camera& camera, const glm::mat4& runtimeTransform) {
    camera.setDirect(runtimeTransform * glm::vec4(eye, 1.0f),
        runtimeTransform * glm::vec4(center, 1.0f), glm::mat3(runtimeTransform) * up, fov);
}

void CameraComponent::update(float dt, EntityState& state) {
    if (activeUuid == uuid)
        copyToCamera(renderMgr->camera, state.runtimeTransform);
}

void CameraComponent::stop() {
}

// lua stuff
static const char* componentMT = "ONO_CameraComponent";
void CameraComponent::registerLuaMetatable() {
    lua_State* state = scripting::getState();
    luaL_newmetatable(state, componentMT);
    // register index op - REQUIRED
    lua_pushvalue(state, -1);
    lua_setfield(state, -2, "__index");
    lua_pop(state, 1);
}

void CameraComponent::pushLuaTable(lua_State* state) {
    lua_createtable(state, 0, 0);
    lua_pushstring(state, name.c_str());
    lua_setfield(state, -2, "name");
    lua_pushinteger(state, uuid);
    lua_setfield(state, -2, "uuid");
    lua_pushlightuserdata(state, this);
    lua_setfield(state, -2, "ptr");
    scripting::pushVec3Ref(state, &eye);
    lua_setfield(state, -2, "eye");
    scripting::pushVec3Ref(state, &center);
    lua_setfield(state, -2, "center");
    scripting::pushVec3Ref(state, &up);
    lua_setfield(state, -2, "up");
    scripting::pushFloatRef(state, &fov);
    lua_setfield(state, -2, "fov");
    luaL_setmetatable(state, componentMT);
}
