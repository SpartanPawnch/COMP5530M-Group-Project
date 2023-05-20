// adapted from https://learnopengl.com/Model-Loading/Model
#include "model.h"

Model::Model() {
}

bool Model::loadModel(const std::string& filename) {
    Assimp::Importer importer;
    uint32_t flags = aiProcess_Triangulate | aiProcess_FixInfacingNormals  | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights;
    const aiScene* scene = importer.ReadFile(filename, flags);
    // this should be handled in a better way later
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        return false;
    }
    std::string backslashSubStr = filename.substr(0, filename.find_last_of('\\'));
    std::string slashSubStr = filename.substr(0, filename.find_last_of('/'));
    directory =
        (slashSubStr.size() > backslashSubStr.size() || backslashSubStr.size() == filename.size())
        ? slashSubStr
        : backslashSubStr;
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
        for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
            vertex.boneId[j] = -1;
            vertex.weight[j] = 0.0f;
        }
        vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial* material = scene->mMaterials[i];
        MATSYS::Material mat;
        mat.name = material->GetName().C_Str();

        defaultMaterialNames.push_back(mat.name);

        aiColor3D color;
        material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
        mat.baseColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_COLOR_EMISSIVE, color);
        mat.emissiveColor = glm::vec3(color.r, color.g, color.b);

        material->Get(AI_MATKEY_SHININESS, mat.roughness);
        material->Get(AI_MATKEY_METALLIC_FACTOR, mat.metalness);
        material->Get(AI_MATKEY_OPACITY, mat.occlusion);
        aiString texturePath;
        if (material->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS) {
            mat.baseColorMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }
        if (material->GetTexture(aiTextureType_SHININESS, 0, &texturePath) == AI_SUCCESS) {
            mat.roughnessMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }
        if (material->GetTexture(aiTextureType_METALNESS, 0, &texturePath) == AI_SUCCESS) {
            mat.metalnessMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }
        if (material->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS) {
            mat.normalMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }
        if (material->GetTexture(aiTextureType_OPACITY, 0, &texturePath) == AI_SUCCESS) {
            mat.alphaMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }
        if (material->GetTexture(aiTextureType_EMISSIVE, 0, &texturePath) == AI_SUCCESS) {
            mat.emissiveMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }
        if (material->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &texturePath) == AI_SUCCESS) {
            mat.occlusionMap = std::string(assetfolder::resolveExternalDependency(texturePath.C_Str(), directory.c_str()));
        }

        MaterialSystem* materialSystem = MaterialSystem::getInstance();
        materialSystem->createMaterialDirectly(mat);
    }
    int matID = mesh->mMaterialIndex;

    for (uint32_t i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        std::string bone_name(bone->mName.C_Str());

        if (boneInfoMap.count(bone_name) == 0) {
            boneInfoMap[bone_name].name = bone_name;
            boneInfoMap[bone_name].id = boneCounter++;
            boneInfoMap[bone_name].transform[0][0] = bone->mOffsetMatrix.a1;
            boneInfoMap[bone_name].transform[1][0] = bone->mOffsetMatrix.a2;
            boneInfoMap[bone_name].transform[2][0] = bone->mOffsetMatrix.a3;
            boneInfoMap[bone_name].transform[3][0] = bone->mOffsetMatrix.a4;
            boneInfoMap[bone_name].transform[0][1] = bone->mOffsetMatrix.b1;
            boneInfoMap[bone_name].transform[1][1] = bone->mOffsetMatrix.b2;
            boneInfoMap[bone_name].transform[2][1] = bone->mOffsetMatrix.b3;
            boneInfoMap[bone_name].transform[3][1] = bone->mOffsetMatrix.b4;
            boneInfoMap[bone_name].transform[0][2] = bone->mOffsetMatrix.c1;
            boneInfoMap[bone_name].transform[1][2] = bone->mOffsetMatrix.c2;
            boneInfoMap[bone_name].transform[2][2] = bone->mOffsetMatrix.c3;
            boneInfoMap[bone_name].transform[3][2] = bone->mOffsetMatrix.c4;
            boneInfoMap[bone_name].transform[0][3] = bone->mOffsetMatrix.d1;
            boneInfoMap[bone_name].transform[1][3] = bone->mOffsetMatrix.d2;
            boneInfoMap[bone_name].transform[2][3] = bone->mOffsetMatrix.d3;
            boneInfoMap[bone_name].transform[3][3] = bone->mOffsetMatrix.d4;
        }

        for (unsigned int j = 0; j < bone->mNumWeights; j++) {
            unsigned int vtx_id = bone->mWeights[j].mVertexId;
            assert(vtx_id <= vertices.size());
            vertices[vtx_id].setBoneData(boneInfoMap[bone_name].id, bone->mWeights[j].mWeight);
        }
    }
     
    return Mesh(meshName,vertices, indices, matID);
}
