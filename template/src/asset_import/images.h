#pragma once
#include <GL/glew.h>
#include <string>
#include <memory>

struct TextureDescriptor {
    GLuint texId;

    std::string* path;
    int idx;
    TextureDescriptor(int _idx, std::string* _path);
    ~TextureDescriptor();
};

// Load a texture with specified path. Returns and internal index to texture.
// Internal index is potentially invalidated whenever textures are unloaded
std::shared_ptr<TextureDescriptor> loadTexture(const char* filename, const std::string& uuid);

// Get texture with uuid specified. Potentially slower,
// prefer index method when possible.
std::shared_ptr<TextureDescriptor> getTexture(const std::string& uuid);

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

int getTextureCount();
