#include "Engine.h"

Engine::Engine()
{
    shouldClose = false;
    currentScene = 0;
    start();
    while (!shouldClose)
    {
        update();
    }
    stop();
}

Engine::~Engine()
{
}

void Engine::start()
{
    //create systems
    //example:
    //time system
    //file system
    //input system
    //render system
    //physics system
    //audio system
    systems.push_back(new InterfaceSystem());

    //TEMPORARY:create scene
    Scene* scene = new Scene();
    //Temporary: create entity
    ModelEntity entity;
    entity.start();
    scene->addEntity(entity);
    CameraEntity camera;
    scene->addEntity(camera);
    scenes.push_back(scene);
    
    openScene(currentScene);
}

void Engine::update()
{
    //poll events
    //get time from time system
    //TEMPOARY: set fake time
    float dt = 0.016f;
    //update systems
    for (int i = 0; i < systems.size(); i++)
    {
        systems[i]->update(dt);
    }
    //update scene
    scenes[currentScene]->update(dt);
    //render scene, last system was not updated before the scene because it should be the render system that will be used to render the scene last
    //systems[systems.size() - 1].update(dt);
}

void Engine::stop()
{
}

void Engine::openScene(int scene) {

}



