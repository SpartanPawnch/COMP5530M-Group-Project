#pragma once

//load level from manifest in path
void loadLevel(const char* path);

//save level to manifest in path
void saveLevel(const char* path);

//save last loaded level
void saveCurrent();

//manager struct for automatic cleanup - mostly handled by asset modules
struct LevelManager {
    bool autoSaveOnQuit = true;
    LevelManager() {}
    ~LevelManager() {
        if (autoSaveOnQuit)
            saveCurrent();
    }
};