#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;
out vec3 WorldPos;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;
uniform sampler2D heightTexture;
uniform float u_MaxHeight;

void main()
{
    TexCoord = aTex;

    float height = (1.0f - texture(heightTexture, TexCoord).r) * u_MaxHeight;
    vec4 pos = vec4(aPos.x, -height, aPos.z, 1.0f);
    //vec4 pos = vec4(aPos, 1.0f);
    WorldPos = (u_MVP.Model * pos).xyz;
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;

}