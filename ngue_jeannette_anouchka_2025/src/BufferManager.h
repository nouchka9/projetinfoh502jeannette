#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <vector>
#include <glad/glad.h> // Existing include for OpenGL
#include "Vertex.h"
#include <iostream>
#include <glm/glm.hpp> // Add this to include GLM

class BufferManager {
public:
    BufferManager() = default;
    ~BufferManager();

    void GenerateBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, 
                         const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    void CreateBuffer(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, 
                      const float* vertices, size_t vertexSize,
                      const unsigned int* indices, size_t indexCount);

private:
    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> EBOs;
};

#endif