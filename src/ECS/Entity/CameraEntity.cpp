#include "CameraEntity.h"

CameraEntity::CameraEntity()
{
    uuid = 200;
    name = "Camera Entity";
    start();
}

CameraEntity::~CameraEntity()
{
    stop();
}

void CameraEntity::start()
{
    //include necessary components for camera in correct order
    //example:
    //transform component
    //camera component
    components.push_back(new TransformComponent());
}

void CameraEntity::update(float dt)
{
    for (int i = 0; i < components.size(); i++)
    {
        components[i]->update(dt);
    }
}

void CameraEntity::stop()
{
}