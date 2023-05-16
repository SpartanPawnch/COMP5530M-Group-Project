#include "ModelEntity.h"

BaseEntity makeModelEntity() {
    BaseEntity ret;
    ret.name = "Model Entity";
    ret.components.addComponent(ModelComponent());
    return ret;
}
