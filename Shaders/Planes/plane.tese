#version 440 core

layout (quads, equal_spacing, ccw) in;

uniform float u_UPatch;
uniform float u_VPatch;

out vec2 TexCoord;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;

vec4 bezier(float t, vec4[4] array)
{
    int size = 3;

    //algorytm de castaljeu
    for (int h = 0; h < size; ++h)
    for (int i = 0; i < size - h;  i++)
    array[i] = (1.0f - t) * array[i] + t * array[i + 1];

    return vec4(array[0].xyz, 1.0f);
}

vec4[4] loadControlPoints(int idx, vec4[16] inArray)
{
    vec4 outArray[4];
    for (int i = 0; i < 4; ++i)
    outArray[i] = inArray[idx + i];

    return outArray;
}

void main()
{
    vec2 patchUV = vec2(mod(gl_PrimitiveID, u_UPatch), floor(gl_PrimitiveID / u_UPatch));
    TexCoord = (gl_TessCoord.xy + patchUV) / vec2(u_UPatch, u_VPatch);

    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 midValue[4];
    vec4 controlPoints[4];
    vec4 inData[16];
    for (int i = 0; i < 16; ++i)
        inData[i] = gl_in[i].gl_Position;

    controlPoints = loadControlPoints(0, inData);
    midValue[0] = bezier(u, controlPoints);
    controlPoints = loadControlPoints(4, inData);
    midValue[1] = bezier(u, controlPoints);
    controlPoints = loadControlPoints(8, inData);
    midValue[2] = bezier(u, controlPoints);
    controlPoints = loadControlPoints(12, inData);
    midValue[3] = bezier(u, controlPoints);

    vec4 pos = bezier(v, midValue);
    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;

}
