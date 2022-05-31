#version 440 core

layout (isolines, equal_spacing, ccw) in;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;

///////////////////////////////////////////////////
// function to evaluate  the Bezier curve
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

void
main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 array[4];
    array[0] = gl_in[0].gl_Position;
    array[1] = gl_in[1].gl_Position;
    array[2] = gl_in[2].gl_Position;
    array[3] = gl_in[3].gl_Position;
    vec4 pos = bezier( u, array);
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;
}