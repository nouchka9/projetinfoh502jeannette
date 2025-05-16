#include "Skybox.h"
#include <stb_image.h>
#include <iostream>

Skybox::Skybox(const std::vector<std::string>& faces) {
    // Vertex data pour le cube de la skybox
    float skyboxVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // Charger les textures du cubemap
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    bool textureLoadSuccess = true;
    for (unsigned int i = 0; i < faces.size(); i++) {
        std::cout << "Loading skybox texture: " << faces[i] << std::endl << std::flush;
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            std::cout << "Loaded skybox texture " << faces[i] << " (Width: " << width << ", Height: " << height << ", Channels: " << nrChannels << ")" << std::endl << std::flush;
            GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "Erreur : échec du chargement de la texture du cubemap : " << faces[i] << std::endl << std::flush;
            if (stbi_failure_reason()) {
                std::cerr << "STB Image Failure Reason: " << stbi_failure_reason() << std::endl << std::flush;
            }
            stbi_image_free(data);
            textureLoadSuccess = false;
        }
    }

    if (!textureLoadSuccess) {
        std::cerr << "Warning: One or more skybox textures failed to load. Skybox may not render correctly." << std::endl << std::flush;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Check for OpenGL errors after texture setup
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after skybox texture setup: " << err << std::endl << std::flush;
    }
}

Skybox::~Skybox() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteTextures(1, &textureID);
}

void Skybox::Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection) {
    // Save current OpenGL state
    GLint previousDepthFunc;
    glGetIntegerv(GL_DEPTH_FUNC, &previousDepthFunc);

    // Configure depth testing for skybox
    glDepthFunc(GL_LEQUAL);
    shader.use();
    glm::mat4 viewNoTranslation = glm::mat4(glm::mat3(view)); // Supprimer la translation
    shader.setMat4("view", viewNoTranslation);
    shader.setMat4("projection", projection);
    glBindVertexArray(VAO);
    //glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    //shader.setInt("skybox", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Restore previous depth function
    glDepthFunc(previousDepthFunc);

    // Check for OpenGL errors after drawing
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL Error after skybox draw: " << err << std::endl << std::flush;
    }
}