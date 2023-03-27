#include "Scene.h"

Scene::Scene()
{
    start();
}

Scene::~Scene()
{
    stop();
}

void Scene::start()
{
    
}

void Scene::update(float dt)
{
    for (int i = 0; i < entities.size(); i++)
    {
        entities[i].update(dt);
    }
}

void Scene::stop()
{
}

void Scene::addEntity(BaseEntity entity)
{
    //generate uuid
    //add uuid to entity
    entities.push_back(entity);
}

void Scene::removeEntity(int uuid)
{
    for (int i = 0; i < entities.size(); i++)
    {
        if (entities[i].uuid == uuid)
        {
            entities.erase(entities.begin() + i);
        }
    }
}


