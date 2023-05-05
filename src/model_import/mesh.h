//adapted from https://learnopengl.com/Model-Loading/Mesh
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "../asset_import/materials.h"

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    //vector of bone ids that influence this vertex and their weights 
    std::vector<unsigned int>boneId;
    std::vector<float>weight;
};

struct BoneInfo
{
    unsigned int id;

    glm::mat4 offset;
};

class Mesh {
public:
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    std::shared_ptr<ActiveMaterial> material;

    Mesh(std::string n, std::vector<Vertex> v, std::vector<unsigned int> i, std::shared_ptr<ActiveMaterial> mat);
    ~Mesh();
    void deleteBuffers();
};  