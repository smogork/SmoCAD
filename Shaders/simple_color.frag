#version 330 core
out vec4 FragColor;
in vec4 vs_FragColor;

void main()
{
    FragColor = vs_FragColor;
    //FragColor = (1.0f, 1.0f, 0.0f, 1.0f);
}