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

void BufferManager::GenerateBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    // Generate and bind VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    VAOs.push_back(VAO);

    // Generate and bind VBO
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    VBOs.push_back(VBO);

    // Generate and bind EBO if indices are provided
    if (!indices.empty()) {
        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        EBOs.push_back(EBO);
    } else {
        EBO = 0;
    }

    // Set vertex attribute pointers
    // Position (location 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    glEnableVertexAttribArray(0);
    // Normal (location 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(1);
    // TexCoords (location 2)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    glEnableVertexAttribArray(2);

    // Unbind VAO
    glBindVertexArray(0);
}