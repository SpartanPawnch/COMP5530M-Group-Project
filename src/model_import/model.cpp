//adapted from https://learnopengl.com/Model-Loading/Model
#include "model.h"

Model::Model() {

}

bool Model::loadModel(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs);
    //this should be handled in a better way later
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }
    directory = filename.substr(0, filename.find_last_of('\\'));
    processNode(scene->mRootNode, scene);
    return true;
}

void Model::deleteBuffers() {
    for (unsigned int i = 0; i < meshes.size(); ++i) {
        meshes[i].deleteBuffers();
    }
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            //no texture coordinates
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    getSkeletonInfo(vertices, mesh, scene);
    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName) {
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
        aiString path;
        mat->GetTexture(type, i, &path);
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); ++j) {
            if (std::strcmp(textures_loaded[j].path.data(), path.C_Str()) == 0) {
                textures.push_back(textures_loaded[j]);
                skip = true;
                break;
            }
        }
        if (!skip) {
            Texture texture;
            //TODO:generate texture id
            texture.id = 1;
            texture.type = typeName;
            texture.path = path.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}

void Model::getSkeletonInfo(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex)
    {
        unsigned int boneID = 0;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            //convert assimp matrix to glm::mat4
            newBoneInfo.offset[0][0] = mesh->mBones[boneIndex]->mOffsetMatrix.a1;
            newBoneInfo.offset[1][0] = mesh->mBones[boneIndex]->mOffsetMatrix.a2;
            newBoneInfo.offset[2][0] = mesh->mBones[boneIndex]->mOffsetMatrix.a3;
            newBoneInfo.offset[3][0] = mesh->mBones[boneIndex]->mOffsetMatrix.a4;
            newBoneInfo.offset[0][1] = mesh->mBones[boneIndex]->mOffsetMatrix.b1;
            newBoneInfo.offset[1][1] = mesh->mBones[boneIndex]->mOffsetMatrix.b2;
            newBoneInfo.offset[2][1] = mesh->mBones[boneIndex]->mOffsetMatrix.b3;
            newBoneInfo.offset[3][1] = mesh->mBones[boneIndex]->mOffsetMatrix.b4;
            newBoneInfo.offset[0][2] = mesh->mBones[boneIndex]->mOffsetMatrix.c1;
            newBoneInfo.offset[1][2] = mesh->mBones[boneIndex]->mOffsetMatrix.c2;
            newBoneInfo.offset[2][2] = mesh->mBones[boneIndex]->mOffsetMatrix.c3;
            newBoneInfo.offset[2][2] = mesh->mBones[boneIndex]->mOffsetMatrix.c4;
            newBoneInfo.offset[0][3] = mesh->mBones[boneIndex]->mOffsetMatrix.d1;
            newBoneInfo.offset[1][3] = mesh->mBones[boneIndex]->mOffsetMatrix.d2;
            newBoneInfo.offset[2][3] = mesh->mBones[boneIndex]->mOffsetMatrix.d3;
            newBoneInfo.offset[3][3] = mesh->mBones[boneIndex]->mOffsetMatrix.d4;

            boneInfoMap[boneName] = newBoneInfo;
            boneID = boneCounter;
            boneCounter++;
        }
        else
        {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
        {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            //add bone to list of bones that influence the vertex and weight
            vertices[vertexId].boneId.push_back(boneID);
            vertices[vertexId].weight.push_back(weight);
        }
    }
}