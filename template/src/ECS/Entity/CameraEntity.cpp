#include "CameraEntity.h"

CameraEntity::CameraEntity() {
    uuid = 200;
    name = "Camera Entity";
    start();
}

CameraEntity::~CameraEntity() {
    stop();
}

void CameraEntity::start() {
    // include necessary components for camera in correct order
    // example:
    // transform component
    // camera component
    components.addComponent(TransformComponent());
}

void CameraEntity::stop() {
}
