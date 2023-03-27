#include "BaseSystem.h"

BaseSystem::BaseSystem()
{
    start();
}

BaseSystem::~BaseSystem()
{
    stop();
}

void BaseSystem::start()
{
}

void BaseSystem::update(float dt)
{
    //each system will have a different update method
}

void BaseSystem::stop()
{
}