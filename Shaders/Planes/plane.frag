#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_ObjectColor;
uniform sampler2D trimTexture;

void main()
{
    float trim = texture(trimTexture, TexCoord).r;
    if (trim != 1.0f)
        discard;

    FragColor = u_ObjectColor;
}