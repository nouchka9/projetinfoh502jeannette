// Model.h
#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Mesh.h"
#include "shaders.h"
#include "BufferManager.h"

class Model {
public:
    Model(const std::string& path, BufferManager& bufferManager, const glm::vec3& position, const glm::vec3& scale, const std::string& texturePath);

    void Draw(Shader& shader, float transparency, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& modelMatrix, bool shadowPass);
    void Render(Shader& shader);
    void SetTransparency(float transparency);
    void SetScale(const glm::vec3& scale);

private:
    std::vector<Mesh> meshes;
    std::vector<float> vertices;
    glm::vec3 position_;
    glm::vec3 scale_;
    float transparency;
    unsigned int VAO, VBO, EBO;
    unsigned int vertexCount;
    unsigned int texture;
    BufferManager& bufferManager_;
};

#endif