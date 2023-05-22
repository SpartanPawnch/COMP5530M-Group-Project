#pragma once
#include "ai.h"
#include "../ECS/Scene/Scene.h"

//this class is defined to help with the ai states for all rigid bodies as the ai state is associated with one rigid body component.
class AIHelper
{


protected:
    AIHelper() {};
    static AIHelper* instance;
public:

    AIHelper(AIHelper& other) = delete;
    void operator=(const AIHelper&) = delete;
    static AIHelper* GetInstance();
    void updateRigidBodies(Scene& scene);
};