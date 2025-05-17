#include "Annotation.h"

Annotation::Annotation(const glm::vec3& position, const std::string& label, const glm::vec3& color)
    : position(position), label(label), color(color) {
    float lineVertices[] = {
        position.x, position.y, position.z,
        position.x, position.y + 1.0f, position.z
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(lineVertices), lineVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

void Annotation::Draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) const {
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    shader.setVec3("color", color);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 2);
    glBindVertexArray(0);
}