#include "SkyBoxComponent.h"
#include <GL/glew.h>
#include <iostream>

static int baseUuid = 0;

SkyBoxComponent::SkyBoxComponent() {
    name = "Skybox Component";
    uuid = baseUuid++;
}
void SkyBoxComponent::start() {
}
void SkyBoxComponent::update(float dt, EntityState& state) {
}
void SkyBoxComponent::stop() {
}

void SkyBoxComponent::updateTex(unsigned int side, std::string path) {
    //update the necessary texture
    std::string uuid = assetfolder::getRelativePath(path.c_str());
    auto desc = getTexture(uuid);

    if (!desc) {
        // load file from disk
        desc = loadTexture(path.c_str(), uuid);
    }

    std::swap(skybox.faces[side].textureDescriptor, desc);
    skybox.faces[side].textureUuid = skybox.faces[side].textureDescriptor ? uuid : "";
    skybox.faces[side].path = skybox.faces[side].textureDescriptor ? path : "";
    //if there are textures for all sides
    if (skybox.faces[0].path != "" && skybox.faces[1].path != "" && skybox.faces[2].path != "" && skybox.faces[3].path != "" && skybox.faces[4].path != "" && skybox.faces[5].path != "") {
        if (skybox.id > 0) {
            glDeleteTextures(1, &skybox.id);
            skybox.id = 0;
        }

        glGenTextures(1, &skybox.id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.id);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        for (unsigned int i = 0; i < 6; i++) {
            glBindTexture(GL_TEXTURE_2D, skybox.faces[i].textureDescriptor->texId);

            int width, height;
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
            glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

            unsigned char* data = new unsigned char[width * height * 4];

            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            
            // Free the pixel data memory
            delete[] data;

            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}