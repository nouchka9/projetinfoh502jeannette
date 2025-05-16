#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shaders.h"

class ParticleSystem {
public:
    struct Particle {
        glm::vec3 position;
        glm::vec3 velocity;
        float life;
    };

    ParticleSystem(unsigned int count);
    ~ParticleSystem();
    void Draw(Shader& shader, const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);

private:
    std::vector<Particle> particles;
    unsigned int VAO, VBO, instanceVBO;
};

#endif