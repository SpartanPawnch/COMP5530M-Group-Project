//adapted from https://learnopengl.com/Model-Loading/Mesh
#include "mesh.h"
#include "../../render-engine/RenderManager.h"

Mesh::Mesh(std::vector<Vertex> v, std::vector<unsigned int> i, std::vector<Texture> t)
{
    vertices = v;
    indices = i;
    textures = t;
    RenderManager::uploadMesh(&vertices, &indices, &VAO, &VBO, &EBO);
}
Mesh::~Mesh() {

}
void Mesh::deleteBuffers() {
    RenderManager::deleteMesh(&VAO, &VBO, &EBO);
}