#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 view;
uniform mat4 projection;
uniform vec3 cameraRight;
uniform vec3 cameraUp;
uniform vec3 particlePos;
uniform float particleSize;

void main() {
    vec3 vertexPosition = particlePos + cameraRight * aPos.x * particleSize + cameraUp * aPos.y * particleSize;
    gl_Position = projection * view * vec4(vertexPosition, 1.0);
    TexCoords = aPos;
}