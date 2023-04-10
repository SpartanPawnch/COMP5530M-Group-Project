//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    //vector of bone ids that influence this vertex and their weights 
    std::vector<unsigned int>boneId;
    std::vector<float>weight;
};

struct BoneInfo
{
    unsigned int id;

    glm::mat4 offset;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class SkeletalMesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    SkeletalMesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
};  