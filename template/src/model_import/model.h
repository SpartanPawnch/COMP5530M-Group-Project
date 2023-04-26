//adapted from https://learnopengl.com/Model-Loading/Model
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"

class Model {
public:
	Model();

    std::vector<Mesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    bool loadModel(const std::string& path);
    void deleteBuffers();
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
};