//adapted from https://learnopengl.com/Model-Loading/Mesh
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include "../asset_import/materials.h"

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    //vector of bone ids that influence this vertex and their weights 
    glm::ivec4 boneId = glm::ivec4(-1);
    glm::vec4 weight = glm::ivec4(0.0);

    void setBoneData(unsigned int id, float w) {
        //std::cout << "---------------------" << std::endl;
        //std::cout << "Setting id:weight " << id << ":" << w << std::endl;
        for (int i = 0; i < MAX_BONE_INFLUENCE; ++i) {
            if (boneId[i] < 0 || boneId[i] == id) {
                boneId[i] = id;
                weight[i] = w;
                //std::cout << "Set to index " << i << std::endl;
                //std::cout << "---------------------" << std::endl;
                return;
            }
        }
        //std::cout << "Not set" << std::endl;
        //std::cout << "---------------------" << std::endl;
        assert(0);
    }
};

class Mesh {
public:
    std::string name;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    std::shared_ptr<ActiveMaterial> material=nullptr;

    Mesh(std::string n, std::vector<Vertex> v, std::vector<unsigned int> i, std::shared_ptr<ActiveMaterial> mat);
    ~Mesh();
    void deleteBuffers();
};  