#include "CameraComponent.h"

#include <algorithm>

static int baseUuid = 0;

int CameraComponent::activeUuid = -1;

CameraComponent::CameraComponent() {
    name = "Camera Component";
    uuid = baseUuid++;
}

CameraComponent::CameraComponent(const std::string& _name, const int _uuid) {
    name = _name;
    uuid = _uuid;
    baseUuid = std::max(baseUuid, _uuid + 1);
}

CameraComponent::~CameraComponent() {
}

void CameraComponent::start() {
}

void CameraComponent::update(float dt) {
}

void CameraComponent::stop() {
}
