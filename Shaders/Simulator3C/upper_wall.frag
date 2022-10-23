#version 330

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D heightTexture;
uniform float u_MaxHeight;

void main() {
    //float height = texture(heightTexture, TexCoord).r * u_MaxHeight;

    FragColor = vec4(TexCoord.r, TexCoord.g, 0.0f, 1.0f);
    //FragColor = vec4(height, height, height, 1.0f);
}
