#include "BufferManager.h"

BufferManager::~BufferManager() {
    for (unsigned int vao : VAOs) {
        glDeleteVertexArrays(1, &vao);
    }
    for (unsigned int vbo : VBOs) {
        glDeleteBuffers(1, &vbo);
    }
    for (unsigned int ebo : EBOs) {
        glDeleteBuffers(1, &ebo);
    }
}

void BufferManager::GenerateBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, 
                                   const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Existing implementation (unchanged)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    VAOs.push_back(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    VBOs.push_back(VBO);

    if (!indices.empty()) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        EBOs.push_back(EBO);
    } else {
        EBO = 0;
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

// New implementation for CreateBuffer
void BufferManager::CreateBuffer(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, 
                                const float* vertices, size_t vertexSize,
                                const unsigned int* indices, size_t indexCount) {
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    VAOs.push_back(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);
    VBOs.push_back(VBO);

    if (indices != nullptr && indexCount > 0) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
        EBOs.push_back(EBO);
    } else {
        EBO = 0;
    }

    // Assuming vertices are raw position data (3 floats per vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}