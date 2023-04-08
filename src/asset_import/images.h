#pragma once
#include <GL/glew.h>
#include <string>

struct TextureInfo {
    int width;
    int height;
    GLuint id;
};

// Load a texture with specified path. Returns and internal index to texture 
int loadTexture(const char* filename, const std::string& uuid);

// Get texture with index specified
const TextureInfo& getTexture(int i);

// Get texture with uuid specified
const TextureInfo& getTexture(const std::string& uuid);

// Delete all currently loaded textures
void clearTextures();
struct TextureManager {
    TextureManager() {}
    ~TextureManager() {
        clearTextures();
    }
};
