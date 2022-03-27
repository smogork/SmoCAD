#version 440 core
layout (location = 0) in vec4 aPos;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;


void main()
{
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * aPos;
}