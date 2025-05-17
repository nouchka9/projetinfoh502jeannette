#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "shaders.h"
#include "Vertex.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int VAO, unsigned int VBO, unsigned int EBO)
        : vertices(vertices), indices(indices), VAO(VAO), VBO(VBO), EBO(EBO) {}

    void Draw(Shader& shader, bool isShadowPass = false) const;
};

#endif