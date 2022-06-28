#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_ObjectColor;
uniform bool u_FlipTrimming;
uniform bool u_ActiveTrimming;
uniform sampler2D trimTexture;

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