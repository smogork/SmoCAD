#version 440 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec4 u_ObjectColor;
uniform sampler2D trimTexture;

void main()
{
    float alpha = texture2D(trimTexture, TexCoord).r;
    FragColor = vec4(u_ObjectColor.rgb, alpha);
}
