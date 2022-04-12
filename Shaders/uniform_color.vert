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
    last_FragColor = u_ObjectColor;

    vec4 pos = vec4(aPos.xyz, 1.0f);
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;
    gl_PointSize = 5;//[TODO] dodac uniform na rozmiar punktu
}