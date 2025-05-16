#ifndef BUFFER_MANAGER_H
#define BUFFER_MANAGER_H

#include <glad/glad.h>
#include <vector>
#include "Vertex.h"

class BufferManager {
public:
    BufferManager() {}
    ~BufferManager();

    void GenerateBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

private:
    std::vector<unsigned int> VAOs;
    std::vector<unsigned int> VBOs;
    std::vector<unsigned int> EBOs;
};

#endif