#include "Model.h"
#include <glad/glad.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.h>
#include <iostream>

Model::Model(const std::string& path, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath) 
    : position(position), scale(scale) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    aiMesh* mesh = scene->mMeshes[0];
    vertexCount = mesh->mNumFaces * 3;

    std::vector<float> vertices;
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            aiVector3D pos = mesh->mVertices[face.mIndices[j]];
            vertices.push_back(pos.x);
            vertices.push_back(pos.y);
            vertices.push_back(pos.z);
            if (mesh->HasTextureCoords(0)) {
                vertices.push_back(mesh->mTextureCoords[0][face.mIndices[j]].x);
                vertices.push_back(mesh->mTextureCoords[0][face.mIndices[j]].y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
            aiVector3D normal = mesh->mNormals[face.mIndices[j]];
            vertices.push_back(normal.x);
            vertices.push_back(normal.y);
            vertices.push_back(normal.z);
        }
    }

    bufferManager.CreateBuffer(VAO, VBO, EBO, vertices.data(), vertices.size() * sizeof(float), nullptr, 0);

    int width, height, nrChannels;
    unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    } else {
        std::cout << "Failed to load texture: " << texturePath << std::endl;
    }
}

void Model::Draw(Shader& shader, float transparency, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMatrix, bool shadowPass) {
    glBindVertexArray(VAO);
    shader.setFloat("transparency", transparency);
    if (!shadowPass) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setInt("texture1", 0);
    }
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    glBindVertexArray(0);
}

void Model::SetTransparency(float transparency) {
    this->transparency = transparency;
}

void Model::SetScale(const glm::vec3& scale) {
    this->scale = scale;
}