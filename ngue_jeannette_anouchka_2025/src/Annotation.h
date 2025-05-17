#ifndef ANNOTATION_H
#define ANNOTATION_H

#include <glm/glm.hpp>
#include <string>
#include "shaders.h"

class Annotation {
public:
    Annotation(const glm::vec3& position, const std::string& label, const glm::vec3& color = glm::vec3(1.0f, 1.0f, 0.0f));
    void Draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) const;

private:
    glm::vec3 position;
    std::string label;
    glm::vec3 color;
    unsigned int VAO, VBO;
};

#endif