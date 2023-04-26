#include "CameraComponent.h"

#include <algorithm>

#include <glm/gtc/matrix_transform.hpp>

#include "../../../render-engine/Camera.h"
#include "../../../render-engine/RenderManager.h"

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
