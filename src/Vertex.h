#ifndef VERTEX_H
#define VERTEX_H

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;

    bool operator==(const Vertex& other) const {
        return Position == other.Position && Normal == other.Normal && TexCoords == other.TexCoords;
    }

    bool operator<(const Vertex& other) const {
        if (Position != other.Position) return Position.x < other.Position.x || Position.y < other.Position.y || Position.z < other.Position.z;
        if (Normal != other.Normal) return Normal.x < other.Normal.x || Normal.y < other.Normal.y || Normal.z < other.Normal.z;
        return TexCoords.x < other.TexCoords.x || TexCoords.y < other.TexCoords.y;
    }
};

#endif