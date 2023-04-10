#pragma once
#include <GL/glew.h>
#include <string>

struct TextureInfo {
    int width;
    int height;
    GLuint id;
};

// Load a texture with specified path. Returns and internal index to texture.
// Internal index is potentially invalidated whenever textures are unloaded
int loadTexture(const char* filename, const std::string& uuid);

// Get texture with index specified
const TextureInfo& getTexture(int i);

// Get texture with uuid specified. Potentially slower,
// prefer index method when possible.
const TextureInfo& getTexture(const std::string& uuid);

// Stop all currently loaded textures from being cleared in
// the program runtime
void setTexturesStaticThreshold();

// Delete all textures except static ones
void clearDynamicTextures();

// Delete all currently loaded textures
void clearAllTextures();
struct TextureManager {
    TextureManager() {
    }
    ~TextureManager() {
        clearDynamicTextures();
    }
};
