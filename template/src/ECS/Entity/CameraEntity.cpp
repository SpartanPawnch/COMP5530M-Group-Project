#include "CameraEntity.h"
#include "../Component/CameraComponent.h"

BaseEntity makeCameraEntity() {
    BaseEntity ret;
    ret.name = "Camera Entity";
    ret.components.addComponent(CameraComponent());
    return ret;
}
