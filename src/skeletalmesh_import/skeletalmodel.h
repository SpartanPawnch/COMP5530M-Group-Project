//adapted from https://learnopengl.com/Model-Loading/Model
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "skeletalmesh.h"

class SkeletalModel {
public:
    SkeletalModel();

    std::vector<SkeletalMesh> meshes;
    std::vector<Texture> textures_loaded;
    std::string directory;

    //skeleton information
    std::map < std:: string, BoneInfo > boneInfoMap;
    unsigned int boneCounter = 0;

    void loadModel(const std::string& path);
    void processNode(aiNode* node, const aiScene* scene);
    SkeletalMesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);

    //skeleton methods
    void getSkeletonInfo(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene);
};