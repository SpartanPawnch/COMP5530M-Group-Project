#include "SkeletalMeshEntity.h"

SkeletalMeshEntity::SkeletalMeshEntity()
{
    start();
}

SkeletalMeshEntity::~SkeletalMeshEntity()
{
    stop();
}

void SkeletalMeshEntity::start()
{
    //include necessary components for model in correct order
    //example:
    //transform component
    //skeletal mesh component
}

void SkeletalMeshEntity::update(float dt)
{
    for (int i = 0; i < components.size(); i++)
    {
        components[i].update(dt);
    }
}

void SkeletalMeshEntity::stop()
{
}