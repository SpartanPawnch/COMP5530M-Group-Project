// adapted from https://learnopengl.com/Model-Loading/Model
#include "model.h"

Model::Model() {
}

bool Model::loadModel(const std::string& filename) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FixInfacingNormals  | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // this should be handled in a better way later
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

    std::string meshName = mesh->mName.C_Str();

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.position =
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords =
                glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            // no texture coordinates
            vertex.texCoords = glm::vec2(0.0f, 0.0f);
        }
        vertex.tangent = glm::vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
        vertex.bitangent = glm::vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    std::shared_ptr<ActiveMaterial> activeMat;
    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        Material mat;
        mat.name = material->GetName().C_Str();

        aiColor3D color;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mat.baseColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        mat.emissiveColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_SHININESS, mat.roughness);
        material->Get(AI_MATKEY_REFLECTIVITY, mat.metalness);
        material->Get(AI_MATKEY_OPACITY, mat.occlusion);

        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            mat.baseColorMap = std::string(texturePath.C_Str());
        }
        if (material->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS) {
            mat.roughnessMap = std::string(texturePath.C_Str());
        }
        if (material->GetTexture(aiTextureType_REFLECTION, 0, &texturePath) == AI_SUCCESS) {
            mat.metalnessMap = std::string(texturePath.C_Str());
        }
        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS) {
            mat.normalMap = std::string(texturePath.C_Str());
        }
        if (material->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS) {
            mat.alphaMap = std::string(texturePath.C_Str());
        }
        if (material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS) {
            mat.emissiveMap = std::string(texturePath.C_Str());
        }
        if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS) {
            mat.occlusionMap = std::string(texturePath.C_Str());
        }

        MaterialSystem* materialSystem = MaterialSystem::getInstance();
        activeMat = materialSystem->loadActiveMaterial(mat);
    }
    getSkeletonInfo(vertices, mesh, scene);
    return Mesh(meshName,vertices, indices, activeMat);
}

void Model::getSkeletonInfo(std::vector<Vertex>& vertices, aiMesh* mesh, const aiScene* scene) {
    for (int boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        unsigned int boneID = 0;
        std::string boneName = mesh->mBones[boneIndex]->mName.C_Str();
        if (boneInfoMap.find(boneName) == boneInfoMap.end()) {
            BoneInfo newBoneInfo;
            newBoneInfo.id = boneCounter;
            // convert assimp matrix to glm::mat4
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
        else {
            boneID = boneInfoMap[boneName].id;
        }
        assert(boneID != -1);
        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId <= vertices.size());
            // add bone to list of bones that influence the vertex and weight
            vertices[vertexId].boneId.push_back(boneID);
            vertices[vertexId].weight.push_back(weight);
        }
    }
}
