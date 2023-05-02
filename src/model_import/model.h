//adapted from https://learnopengl.com/Model-Loading/Model
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "../asset_import/images.h"
#include "../asset_import/folders.h"
#include "mesh.h"

class Model {
public:
	Model();

    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    //skeleton information
    std::map < std::string, BoneInfo > boneInfoMap;
    unsigned int boneCounter = 0;

    bool loadModel(const std::string& path);
    void deleteBuffers();
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

    //skeleton methods
    void getSkeletonInfo(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};