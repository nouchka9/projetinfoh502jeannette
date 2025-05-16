#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <string>
#include "shaders.h"
#include "BufferManager.h"

class Skybox {
public:
    Skybox(const std::vector<std::string>& faces);
    ~Skybox();
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection);

private:
    unsigned int VAO, VBO;
    unsigned int textureID;
};

#endif