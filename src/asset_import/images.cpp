#include "images.h"
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../../external/stb_image.h"

#include <vector>
#include <map>
#include <string>
#include <memory>

#include <cstdio>

#include "../logging.h"

// --- Internal State ---
struct TextureInfo {
    // redundant info for easy deletion
    std::string path;
    std::string uuid;
    std::weak_ptr<TextureDescriptor> ref;

    // texture info
    GLuint texId;

    TextureInfo() {
        texId = 0;
    }

    // brace init constructor
    TextureInfo(const std::string& _uuid, const std::string& _path, GLuint _id,
        std::weak_ptr<TextureDescriptor> _ref)
        : uuid(_uuid), path(_path), texId(_id), ref(_ref) {
    }

    // copy constructor
    TextureInfo(TextureInfo& aInfo) {
        // copy as normal
        uuid = aInfo.uuid;
        path = aInfo.path;
        texId = aInfo.texId;
        ref = aInfo.ref;

        // fix path reference
        if (ref.lock())
            ref.lock()->path = &path;
    }

    TextureInfo& operator=(TextureInfo& aInfo) {
        // copy as normal
        uuid = aInfo.uuid;
        path = aInfo.path;
        texId = aInfo.texId;
        ref = aInfo.ref;

        // fix path reference
        if (ref.lock())
            ref.lock()->path = &path;

        return *this;
    }

    // move constructor
    TextureInfo(TextureInfo&& aInfo) {
        // copy as normal
        uuid = std::move(aInfo.uuid);
        path = std::move(aInfo.path);
        texId = std::move(aInfo.texId);
        ref = std::move(aInfo.ref);

        // fix path reference
        if (ref.lock())
            ref.lock()->path = &path;
    }

    // move assignment
    TextureInfo& operator=(TextureInfo&& aInfo) {
        // copy as normal
        uuid = std::move(aInfo.uuid);
        path = std::move(aInfo.path);
        texId = std::move(aInfo.texId);
        ref = std::move(aInfo.ref);

        // fix path reference
        if (ref.lock())
            ref.lock()->path = &path;

        return *this;
    }
};

static std::unordered_map<std::string, int> uuidToIdx;
static std::vector<TextureInfo> loadedTextures;

// --- TextureDescriptor Methods ---
TextureDescriptor::TextureDescriptor(int _idx, std::string* _path) : idx(_idx), path(_path) {
}

TextureDescriptor::~TextureDescriptor() {
    // skip if bulk clear has been performed
    if (loadedTextures.size() <= idx)
        return;

    // clear texture

    glDeleteTextures(1, &loadedTextures[idx].texId);

    // clear related index
    uuidToIdx.erase(loadedTextures[idx].uuid);

    // update info for back
    uuidToIdx[loadedTextures.back().uuid] = idx;
    if (auto ref = loadedTextures.back().ref.lock()) {
        ref->idx = idx;
        ref->path = &loadedTextures[idx].path;
    }

    // move deleted clip to back
    std::swap(loadedTextures[idx], loadedTextures.back());

    // clear audio clip
    loadedTextures.pop_back();
}

// --- Module Functions ---

std::shared_ptr<TextureDescriptor> loadTexture(const char* filename, const std::string& uuid) {

    int width, height, channels;

    // try to load data
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);
    if (data == NULL) {
        logging::logErr("Failed to load texture {}\n", filename);
        return std::shared_ptr<TextureDescriptor>();
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
            loadedTextures.emplace_back(TextureInfo{
                uuid, std::string(filename), texID, std::weak_ptr<TextureDescriptor>()});
            std::shared_ptr<TextureDescriptor> desc = std::make_shared<TextureDescriptor>(
                int(loadedTextures.size() - 1), &loadedTextures.back().path);
            loadedTextures.back().ref = std::weak_ptr<TextureDescriptor>(desc);
            uuidToIdx[uuid] = loadedTextures.size() - 1;
            desc->texId = texID;
            return desc;
        }

        // replace existing otherwise
        int idx = uuidToIdx[uuid];
        glDeleteTextures(1, &loadedTextures[idx].texId);
        loadedTextures[idx].texId = texID;
    }

    // something went wrong - return failure
    logging::logErr("Failed to load texture {}\n", filename);
    return std::shared_ptr<TextureDescriptor>();
}

std::shared_ptr<TextureDescriptor> getTexture(const std::string& uuid) {
    if (uuidToIdx.count(uuid) == 0)
        return std::shared_ptr<TextureDescriptor>();
    int idx = uuidToIdx[uuid];
    return std::shared_ptr<TextureDescriptor>(loadedTextures[idx].ref);
}

static unsigned int staticThreshold = 0;

void setTexturesStaticThreshold() {
    staticThreshold = loadedTextures.size();
}

void clearDynamicTextures() {
    for (unsigned int i = staticThreshold; i < loadedTextures.size(); i++)
        glDeleteTextures(1, &loadedTextures[i].texId);
    loadedTextures.resize(staticThreshold);
    // TODO clear map appropriately
}

void clearAllTextures() {
    for (unsigned int i = 0; i < loadedTextures.size(); i++)
        glDeleteTextures(1, &loadedTextures[i].texId);
    loadedTextures.clear();
    uuidToIdx.clear();
}

int getTextureCount() {
    return loadedTextures.size();
}
