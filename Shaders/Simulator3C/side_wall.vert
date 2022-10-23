#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in float aIgnoreHeight;

out vec2 TexCoord;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform sampler2D heightTexture;
uniform MVP u_MVP;
uniform float u_MaxHeight;

void main()
{
    TexCoord = aTex;
    float height;

    if (aIgnoreHeight > 0)
        height = texture(heightTexture, TexCoord).r * u_MaxHeight;
    else
        height = aPos.y;

    vec4 pos = vec4(aPos.x, height, aPos.z, 1.0f);
    //vec4 pos = vec4(aPos, 1.0f);
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;

}