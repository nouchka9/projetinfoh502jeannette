#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <glm/glm.hpp>
#include "shaders.h"
#include "BufferManager.h"

class Model {
public:
    Model(const std::string& path, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath);
    void Draw(Shader& shader, float transparency, const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& projection = glm::mat4(1.0f), const glm::mat4& modelMatrix = glm::mat4(1.0f), bool shadowPass = false);
    void SetTransparency(float transparency);
    void SetScale(const glm::vec3& scale);

    glm::vec3 scale;
    glm::vec3 position;

private:
    unsigned int VAO, VBO, EBO, texture;
    int vertexCount;
    float transparency = 1.0f;
};

#endif