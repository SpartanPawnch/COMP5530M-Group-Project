#include "SkeletalMeshEntity.h"

SkeletalMeshEntity::SkeletalMeshEntity() {
    uuid = 500;
    name = "Skeletal Mesh Entity";
    start();
}

SkeletalMeshEntity::~SkeletalMeshEntity() {
    stop();
}

void SkeletalMeshEntity::start() {
    // include necessary components for model in correct order
    // example:
    // transform component
    // skeletal mesh component
}

void SkeletalMeshEntity::stop() {
}
