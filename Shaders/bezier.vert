#version 440 core
layout (location = 0) in vec4 aPos;




void main()
{
    //gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * aPos;
    gl_Position = aPos;
}