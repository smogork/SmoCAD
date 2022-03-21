#version 330 core
layout (location = 0) in vec4 aPos;
layout (location = 1) in vec3 aColor;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;

out vec4 vs_FragColor;

void main()
{
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * aPos;
    vs_FragColor = vec4(aColor, 1.0f);
}