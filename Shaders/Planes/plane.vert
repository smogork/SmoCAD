#version 440 core
layout (location = 0) in vec4 aPos;

void main()
{
    gl_Position = vec4(aPos.xyz, 1.0f);
}