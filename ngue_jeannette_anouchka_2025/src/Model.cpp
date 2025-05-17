#include "Model.h"
#include <iostream>
#define GLM_ENABLE_EXPERIMENTAL // Add this before including GLM headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "stb_image.h" // Updated to reflect the file's location in src/
#include "shaders.h" // Include shaders.h for Shader type


Model::Model(const std::string& path, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath)
    : position_(position), scale_(scale), transparency(1.0f), bufferManager_(bufferManager) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        vertexCount = mesh->mNumFaces * 3;

        for (unsigned int j = 0; j < mesh->mNumVertices; j++) {
            vertices.push_back(mesh->mVertices[j].x);
            vertices.push_back(mesh->mVertices[j].y);
            vertices.push_back(mesh->mVertices[j].z);

            if (mesh->HasNormals()) {
                vertices.push_back(mesh->mNormals[j].x);
                vertices.push_back(mesh->mNormals[j].y);
                vertices.push_back(mesh->mNormals[j].z);
            }

            if (mesh->mTextureCoords[0]) {
                vertices.push_back(mesh->mTextureCoords[0][j].x);
                vertices.push_back(mesh->mTextureCoords[0][j].y);
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }

        bufferManager.CreateBuffer(VAO, VBO, EBO, vertices.data(), vertices.size() * sizeof(float), nullptr, 0);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        int width, height, nrChannels;
        unsigned char* data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture at " << texturePath << std::endl;
        }
    }
}

void Model::Draw(Shader& shader, float transparency, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMatrix, bool shadowPass) {
    glBindVertexArray(VAO);
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setMat4("model", modelMatrix);
    if (!shadowPass) {
        glBindTexture(GL_TEXTURE_2D, texture);
        shader.setFloat("transparency", transparency);
    }
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}

void Model::Render(Shader& shader) {
    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position_);
    modelMatrix = glm::scale(modelMatrix, scale_);
    Draw(shader, transparency, glm::mat4(1.0f), glm::mat4(1.0f), modelMatrix, false);
}

void Model::SetTransparency(float transparency) {
    this->transparency = transparency;
}

void Model::SetScale(const glm::vec3& scale) {
    this->scale_ = scale;
}