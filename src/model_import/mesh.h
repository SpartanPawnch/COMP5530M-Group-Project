//adapted from https://learnopengl.com/Model-Loading/Mesh
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <assimp/scene.h>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    unsigned int VAO, VBO, EBO;

    Mesh(std::vector<Vertex> v, std::vector<unsigned int> i, std::vector<Texture> t);
    ~Mesh();
    void deleteBuffers();
};  