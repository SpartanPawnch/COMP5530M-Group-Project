#pragma once
#include<string>

#include "ECS/Scene/Scene.h"

//load level from manifest in path
void loadLevel(const char* path, Scene& scene);

//save level to manifest in path
void saveLevel(const char* path, const Scene& scene);

//save last loaded level
void saveCurrentLevel();

//load default project from project file
//returns relative path to default level, or empty string on error
std::string loadProjectFile(const char* path);

//save project file to manifest
void saveProjectFile(const char* path);

//manager struct for automatic cleanup - mostly handled by asset modules
struct LevelManager {
    bool autoSaveOnQuit = true;
    LevelManager() {}
    ~LevelManager() {
        if (autoSaveOnQuit)
            saveCurrentLevel();
    }
};