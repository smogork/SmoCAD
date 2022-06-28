#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D trimTexture;
uniform vec4 u_ObjectColor;

void main()
{
    float trim = texture(trimTexture, TexCoord).r;
    if (trim != 1.0f)
        discard;

    FragColor = u_ObjectColor;
}