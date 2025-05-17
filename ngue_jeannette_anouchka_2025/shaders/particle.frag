#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

void main() {
    float dist = length(TexCoords.xy);
    if (dist > 0.5) discard;
    FragColor = vec4(0.0, 1.0, 0.0, 0.5);
}