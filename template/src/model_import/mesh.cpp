//adapted from https://learnopengl.com/Model-Loading/Mesh
#include "mesh.h"
#include "../../render-engine/RenderManager.h"

Mesh::Mesh(std::string n, std::vector<Vertex> v, std::vector<unsigned int> i, int matID)
{
    name = n;
    vertices = v;
    indices = i;
    defaultMaterialID = matID;
    RenderManager::uploadMesh(&vertices, &indices, &VAO, &VBO, &EBO);
}
Mesh::~Mesh() {

}
void Mesh::deleteBuffers() {
    RenderManager::deleteMesh(&VAO, &VBO, &EBO);
}