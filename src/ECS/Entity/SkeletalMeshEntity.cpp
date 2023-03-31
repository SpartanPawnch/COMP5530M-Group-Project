#include "SkeletalMeshEntity.h"

SkeletalMeshEntity::SkeletalMeshEntity()
{
    uuid = 500;
    name = "Skeletal Mesh Entity";
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
    for (unsigned int i = 0; i < components.size(); i++)
    {
        components[i]->update(dt);
    }
}

void SkeletalMeshEntity::stop()
{
}