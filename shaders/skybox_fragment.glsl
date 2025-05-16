#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

void main() {
    vec4 color = texture(skybox, TexCoords);
    if (color.a < 0.1 || textureSize(skybox, 0).x <= 0) {
        // Fallback to green if texture fails or is invalid
        color = vec4(0.0, 1.0, 0.0, 1.0);
    }
    FragColor = color;
}