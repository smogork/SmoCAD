#version 440 core
out vec4 FragColor;
in vec4 last_FragColor;

void main()
{
    FragColor = last_FragColor;
    //FragColor = (1.0f, 1.0f, 0.0f, 1.0f);
}