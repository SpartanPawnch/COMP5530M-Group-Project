//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#include "skeletalmesh.h"

SkeletalMesh::SkeletalMesh(std::vector<SkeletalVertex> vertices, std::vector<unsigned int> indices, std::vector<SkeletalTexture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
}