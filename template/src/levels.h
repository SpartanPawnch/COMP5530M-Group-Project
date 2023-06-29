#pragma once
#include <string>

#include "ECS/Scene/Scene.h"

// load level from manifest in path
void loadLevel(const char* path, Scene& scene);

// load default project from project file
// returns relative path to default level, or empty string on error
std::string loadProjectFile(const char* path);

// manager struct for automatic cleanup - mostly handled by asset modules
struct LevelManager {
    LevelManager() {
    }
    ~LevelManager() {
    }
};
