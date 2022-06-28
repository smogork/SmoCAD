#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_ObjectColor;
uniform int u_FlipTrimming;
uniform sampler2D trimTexture;

void main()
{
    float trim = texture(trimTexture, TexCoord).r;
    if (u_FlipTrimming != 0)
        trim = 1.0f - trim;

    if (trim != 1.0f)
        discard;
    FragColor = u_ObjectColor;
}