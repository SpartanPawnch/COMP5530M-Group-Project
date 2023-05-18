#pragma once
struct ComponentLocation{
    int entityUuid;
    int componentIdx;
    enum CompType{
        BASECOMPONENT,
        PLAYERCONTROLLERCOMPONENT,
        SCRIPTCOMPONENT,
        CAMERACOMPONENT,
        AUDIOSOURCECOMPONENT,
        MODELCOMPONENT,
        SKELETALMODELCOMPONENT,
        LIGHTCOMPONENT,
        SKYBOXCOMPONENT,
        RIGIDBODYCOMPONENT,
        CONTROLLERCOMPONENT,
        TRANSFORMCOMPONENT,
        COMPTYPE_MAX
    };
    CompType type;
};
