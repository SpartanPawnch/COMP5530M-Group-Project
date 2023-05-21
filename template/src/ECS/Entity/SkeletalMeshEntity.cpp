#include "SkeletalMeshEntity.h"

BaseEntity makeSkeletalMeshEntity() {
    BaseEntity ret;
    ret.name = "Skeletal Mesh Entity";
    ret.components.addComponent(SkeletalModelComponent());
    return ret;
}
