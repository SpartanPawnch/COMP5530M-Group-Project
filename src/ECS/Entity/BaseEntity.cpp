#include "BaseEntity.h"

BaseEntity::BaseEntity()
{
    uuid = 10;
    name = "Base Entity";
    start();
}

BaseEntity::~BaseEntity()
{
    stop();
}

void BaseEntity::start()
{
}

void BaseEntity::update(float dt)
{
    for (int i = 0; i < components.size(); i++)
    {
        components[i]->update(dt);
    }
}

void BaseEntity::stop()
{
}

void BaseEntity::removeComponent(int uuid)
{
    for (int i = 0; i < components.size(); i++)
    {
        if (components[i]->uuid == uuid)
        {
            components.erase(components.begin() + i);
        }
    }
}