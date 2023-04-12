#include "ModelEntity.h"

ModelEntity::ModelEntity() {
    uuid = 100;
    name = "Model Entity";
    start();
}

ModelEntity::~ModelEntity() {
    stop();
}

void ModelEntity::start() {
    // include necessary components for model in correct order
    // example:
    // transform component
    // model component
}

void ModelEntity::stop() {
}
