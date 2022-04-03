#version 440 core
layout (location = 0) in vec4 aPos;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;
uniform vec4 u_ObjectColor;

out vec4 last_FragColor;

void main()
{
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * aPos;
    gl_PointSize = 5;//[TODO] dodac uniform na rozmiar punktu
    last_FragColor = u_ObjectColor;
}