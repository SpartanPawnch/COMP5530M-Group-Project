#include "ModelEntity.h"

ModelEntity::ModelEntity()
{
    start();
}

ModelEntity::~ModelEntity()
{
    stop();
}

void ModelEntity::start()
{
    //include necessary components for model in correct order
    //example:
    //transform component
    //model component
}

void ModelEntity::update(float dt)
{
    for (int i = 0; i < components.size(); i++)
    {
        components[i].update(dt);
    }
}

void ModelEntity::stop()
{
}