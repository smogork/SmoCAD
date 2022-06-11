#version 330
layout (location = 0) in vec4 aPos;

void main()
{
    vec4 pos = vec4(aPos.xyz, 1.0f);
    gl_Position = pos;
}