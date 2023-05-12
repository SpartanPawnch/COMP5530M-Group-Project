//adapted from https://learnopengl.com/Model-Loading/Mesh
#include "mesh.h"
#include "../../render-engine/RenderManager.h"

Mesh::Mesh(std::string n, std::vector<Vertex> v, std::vector<unsigned int> i, std::shared_ptr<ActiveMaterial> mat)
{
    name = n;
    vertices = v;
    indices = i;
    material = mat;
    RenderManager::uploadMesh(&vertices, &indices, &VAO, &VBO, &EBO);
}
Mesh::~Mesh() {

}
void Mesh::deleteBuffers() {
    RenderManager::deleteMesh(&VAO, &VBO, &EBO);
}