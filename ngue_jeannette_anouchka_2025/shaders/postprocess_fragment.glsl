#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    vec4 color = texture(screenTexture, TexCoords);
    if (textureSize(screenTexture, 0).x <= 0) {
        // Fallback to gray if texture is invalid
        color = vec4(0.5, 0.5, 0.5, 1.0);
    }
    FragColor = color;
}