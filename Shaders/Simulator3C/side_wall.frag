#version 330

in vec2 TexCoord;

out vec4 FragColor;

void main() {
    FragColor = vec4(TexCoord.r, TexCoord.g, 0.0f, 1.0f);
}
