#include "levels.h"

#include<string>

#include<rapidjson/document.h>

static std::string currentPath;


//load level from manifest in path
void loadLevel(const char* path) {
    //load document

    //clear all assets

    //TODO - load audio

    //TODO - load models

    //TODO - load textures

    //TODO - load scripts

    //TODO - load misc

    //TODO - load ECS

    //set level to current if everything is succesful
    currentPath = std::string(path);
}

//save level to manifest in path
void saveLevel(const char* path) {
    //open document for writing

    //TODO - save audio

    //TODO - save models

    //TODO - save textures

    //TODO - save scripts

    //TODO - save misc

    //TODO - save ECS

    //close document
}

//save last loaded level
void saveCurrent() {
    if (!currentPath.empty())
        saveLevel(currentPath.c_str());
}