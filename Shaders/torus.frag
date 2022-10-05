#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D trimTexture;
uniform bool u_FlipTrimming;
uniform bool u_ActiveTrimming;
uniform vec4 u_ObjectColor;

void main()
{
    if (u_ActiveTrimming)
    {
        float trim = texture(trimTexture, TexCoord).r;
        if (u_FlipTrimming)
            trim = 1.0f - trim;
        if (trim != 1.0f)
            discard;
    }

    FragColor = u_ObjectColor;
}