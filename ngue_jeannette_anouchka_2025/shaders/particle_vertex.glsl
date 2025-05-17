#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords; // Add texture coordinates

out vec2 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 particlePos;

void main() {
    vec3 pos = aPos + particlePos;
    TexCoords = aTexCoords; // Pass texture coordinates
    gl_Position = projection * view * vec4(pos, 1.0);
}