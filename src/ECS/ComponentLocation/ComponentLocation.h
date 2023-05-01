#pragma once
struct ComponentLocation{
    int entityIdx;
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
        COMPTYPE_MAX
    };
    CompType type;
};
