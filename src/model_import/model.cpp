// adapted from https://learnopengl.com/Model-Loading/Model
#include "model.h"

Model::Model() {
}

bool Model::loadModel(const std::string& filename) {
    Assimp::Importer importer;
    uint32_t flags = aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights;
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
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vertex;
        vertex.position =
            glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        for (int j = 0; j < MAX_BONE_INFLUENCE; j++) {
            vertex.boneId[j] = -1;
            vertex.weight[j] = 0.0f;
        }
        if (mesh->mTextureCoords[0]) {
            vertex.texCoords =
                glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else {
            // no texture coordinates
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
        std::vector<Texture> diffuseMaps =
            loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture> specularMaps =
            loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
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

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type,
    const std::string& typeName) {
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
            std::string resolvedPath =
                assetfolder::resolveExternalDependency(path.C_Str(), directory.c_str());

            // TODO:generate texture id
            std::string uuid = assetfolder::getRelativePath(resolvedPath.c_str());
            texture.textureDescriptor = loadTexture(resolvedPath.c_str(), uuid);
            texture.id = 1;
            texture.type = typeName;
            texture.path = resolvedPath.c_str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
        }
        else {
            Texture texture;
            texture.id = 0;
            textures_loaded.push_back(texture);
        }
    }
    return textures;
}
