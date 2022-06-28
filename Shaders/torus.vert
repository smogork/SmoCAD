#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

out vec2 TexCoord;

struct MVP
    {
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;

void main()
{
    vec4 pos = vec4(aPos, 1.0f);
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;
    TexCoord = aTex;
}