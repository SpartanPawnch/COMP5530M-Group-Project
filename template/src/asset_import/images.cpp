#include "images.h"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

#include <vector>
#include <map>
#include <string>
#include <cstdio>

#include "../logging.h"

static std::unordered_map<std::string, int> uuidToIdx;
static std::vector<TextureInfo> textures;

int loadTexture(const char* filename, const std::string& uuid) {

    int width, height, channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    // TODO proper log function
    if (data == NULL) {
        logging::logErr("Failed to load texture {}\n", filename);
        return -1;
    }

    // choose image format
    GLint format;
    switch (channels) {
    case 1:
        format = GL_R;
        break;
    case 2:
        format = GL_RG;
        break;
    case 3:
        format = GL_RGB;
        break;
    case 4:
    default:
        format = GL_RGBA;
    };

    // create opengl texture
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    stbi_image_free(data);

    // add texture to array if valid
    if (texID != 0) {
        // try to add new texture
        if (uuidToIdx.count(uuid) == 0) {
            textures.emplace_back(TextureInfo{width, height, texID});
            uuidToIdx[uuid] = textures.size() - 1;
            return (textures.size() - 1);
        }

        // replace existing otherwise
        int idx = uuidToIdx[uuid];
        glDeleteTextures(1, &textures[idx].id);
        textures[idx].id = texID;
        textures[idx].width = width;
        textures[idx].height = height;
    }

    // something went wrong - return failure
    logging::logErr("Failed to load texture {}\n", filename);
    return -1;
}

const TextureInfo& getTexture(int i) {
    return textures[i];
}

const TextureInfo& getTexture(const std::string& uuid) {
    int idx = uuidToIdx[uuid];
    return textures[idx];
}

static unsigned int staticThreshold = 0;

void setTexturesStaticThreshold() {
    staticThreshold = textures.size();
}

void clearDynamicTextures() {
    for (unsigned int i = staticThreshold; i < textures.size(); i++)
        glDeleteTextures(1, &textures[i].id);
    textures.resize(staticThreshold);
    // TODO clear map appropriately
}

void clearAllTextures() {
    for (unsigned int i = 0; i < textures.size(); i++)
        glDeleteTextures(1, &textures[i].id);
    textures.clear();
    uuidToIdx.clear();
}
