#version 330 core
out vec4 FragColor;

in vec2 TexCoords; // Assuming texture coordinates are passed (add to vertex shader if missing)
uniform sampler2D particleTexture;

void main() {
    vec4 color = texture(particleTexture, TexCoords);
    if (color.a < 0.1 || textureSize(particleTexture, 0).x <= 0) {
        // Fallback to yellow if texture fails or is invalid
        color = vec4(1.0, 1.0, 0.0, 1.0);
    }
    FragColor = color;
}