#include "ParticleSystem.h"
#include <cstdlib>

ParticleSystem::ParticleSystem(unsigned int count) : particles(count) {
    // Initialiser les particules
    for (auto& particle : particles) {
        particle.position = glm::vec3(
            (float)rand() / RAND_MAX * 2.0f - 1.0f,
            (float)rand() / RAND_MAX * 2.0f - 1.0f,
            (float)rand() / RAND_MAX * 2.0f - 1.0f
        );
        particle.velocity = glm::vec3(0.0f, 0.1f, 0.0f);
        particle.life = 1.0f;
    }

    // Quad pour les particules
    float quadVertices[] = {
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f
    };

    // Buffer pour les positions des instances
    std::vector<glm::vec3> instancePositions;
    for (const auto& particle : particles) {
        instancePositions.push_back(particle.position);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &instanceVBO);

    glBindVertexArray(VAO);

    // Vertex buffer pour le quad
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Attributs de vertex (position et coordonnées de texture)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    // Buffer pour les positions des instances
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), instancePositions.data(), GL_STATIC_DRAW);

    // Attribut pour les positions des instances
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glVertexAttribDivisor(2, 1); // Indique que cet attribut change par instance

    glBindVertexArray(0);
}

ParticleSystem::~ParticleSystem() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &instanceVBO);
}

void ParticleSystem::Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("cameraPos", cameraPos);

    glBindVertexArray(VAO);

    // Mettre à jour les positions des instances
    std::vector<glm::vec3> instancePositions;
    for (const auto& particle : particles) {
        if (particle.life > 0.0f) {
            instancePositions.push_back(particle.position);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instancePositions.size() * sizeof(glm::vec3), instancePositions.data(), GL_STATIC_DRAW);

    // Dessiner avec instancing
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, instancePositions.size());

    glBindVertexArray(0);
}