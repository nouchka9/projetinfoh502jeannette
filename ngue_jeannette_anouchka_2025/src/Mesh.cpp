#include "Mesh.h"

void Mesh::Draw(Shader& shader, bool isShadowPass) const {
    glBindVertexArray(VAO);
    if (isShadowPass) {
        // Only enable position attribute for shadow pass
        glEnableVertexAttribArray(0); // Position
        // Disable other attributes to avoid mismatch
        glDisableVertexAttribArray(1); // Normal
        glDisableVertexAttribArray(2); // TexCoord
    } else {
        // Enable all attributes for normal rendering
        glEnableVertexAttribArray(0); // Position
        glEnableVertexAttribArray(1); // Normal
        glEnableVertexAttribArray(2); // TexCoord
    }
    if (!indices.empty()) {
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());
    }
    glBindVertexArray(0);
}