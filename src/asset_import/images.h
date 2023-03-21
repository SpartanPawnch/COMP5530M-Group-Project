#pragma once
#include <GL/glew.h>

struct TextureInfo {
    int width;
    int height;
    GLuint id;
};

// Load a texture with specified path. Returns and internal index to texture 
int loadTexture(const char* filename);

// Get texture with index specified
const TextureInfo& getTexture(int i);

// Delete all currently loaded textures
void clearTextures();