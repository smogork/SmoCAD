#version 440 core

layout (isolines, equal_spacing, ccw) in;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;

vec4 bezier(float u, vec4[4] array)
{
    //Wyznaczneie stopnia wielomianu
    int size = 3;
    for (;array[size].w == 0.0f && size > 0; --size);

    //algorytm de castaljeu
    for (int h = 0; h < size; ++h)
    for (int i = 0; i < size - h;  i++)
    array[i] = (1.0f - u) * array[i] + u * array[i + 1];

    return vec4(array[0].xyz, 1.0f);
}

vec4[4] bspline_to_bernstein(vec4[4] array)
{
    vec4 bernsteinPoints[4];
    bernsteinPoints[0] = 1.0f / 6.0f * array[0] + 2.0f / 3.0f * array[1] + 1.0f / 6.0f * array[2];
    bernsteinPoints[1] = 2.0f / 3.0f * array[1] + 1.0f / 3.0f * array[2];
    bernsteinPoints[2] = 1.0f / 3.0f * array[1] + 2.0f / 3.0f * array[2];
    bernsteinPoints[3] = 1.0f / 6.0f * array[1] + 2.0f / 3.0f * array[2] + 1.0f / 6.0f * array[3];

    return bernsteinPoints;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 bsplinePoints[4];
    bsplinePoints[0] = gl_in[0].gl_Position;
    bsplinePoints[1] = gl_in[1].gl_Position;
    bsplinePoints[2] = gl_in[2].gl_Position;
    bsplinePoints[3] = gl_in[3].gl_Position;
    vec4 berstein[4] = bspline_to_bernstein(bsplinePoints);
    vec4 pos = bezier(u, berstein);

    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;
}