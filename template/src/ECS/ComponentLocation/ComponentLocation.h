#pragma once
struct ComponentLocation{
    int entityUuid;
    int componentIdx;
    enum CompType{
        BASECOMPONENT,
        PLAYERCONTROLLERCOMPONENT,
        RIGIDBODYCOMPONENT,
        SCRIPTCOMPONENT,
        CAMERACOMPONENT,
        AUDIOSOURCECOMPONENT,
        MODELCOMPONENT,
        SKELETALMODELCOMPONENT,
        LIGHTCOMPONENT,
        DIRECTIONALLIGHTCOMPONENT,
        SKYBOXCOMPONENT,
        CONTROLLERCOMPONENT,
        TRANSFORMCOMPONENT,
        COMPTYPE_MAX
    };
    CompType type;
};
