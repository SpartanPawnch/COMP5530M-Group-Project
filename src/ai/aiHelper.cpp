#include "aiHelper.h"




AIHelper* AIHelper::GetInstance()
{
    if (instance == nullptr) {
        instance = new AIHelper();
    }
    return instance;
}

void AIHelper::updateRigidBodies(Scene& scene)
{
    State* state;
    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        if (scene.entities[i].components.vecRigidBodyComponent.size() == 0) {
            continue;
        }
        if (scene.entities[i].components.vecRigidBodyComponent[0].aiEnabled)
        {
            state = scene.entities[i].components.vecRigidBodyComponent[0].getStateMachineState();
            RigidBody* rigidBody = scene.entities[i].components.vecRigidBodyComponent[0].rigidBody;
            if (state != nullptr && rigidBody != nullptr)
            {
                state->updateState(rigidBody);
                scene.entities[i].components.vecRigidBodyComponent[0].performStateChange();
            }

        }
    }
    
}