#version 440 core
out vec4 FragColor;
uniform vec4 u_ObjectColor;

void main()
{
    FragColor = u_ObjectColor;
    //FragColor = (1.0f, 1.0f, 0.0f, 1.0f);
}
