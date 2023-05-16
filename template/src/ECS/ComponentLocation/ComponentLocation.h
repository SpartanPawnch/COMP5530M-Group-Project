#pragma once
struct ComponentLocation{
    int entityUuid;
    int componentIdx;
    enum CompType{
        BASECOMPONENT,
        TRANSFORMCOMPONENT,
        SCRIPTCOMPONENT,
        CAMERACOMPONENT,
        AUDIOSOURCECOMPONENT,
        MODELCOMPONENT,
        SKELETALMODELCOMPONENT,
        LIGHTCOMPONENT,
        SKYBOXCOMPONENT,
        COMPTYPE_MAX
    };
    CompType type;
};
