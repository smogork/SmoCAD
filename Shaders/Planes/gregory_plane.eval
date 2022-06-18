#version 440 core

layout (quads, equal_spacing, ccw) in;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;

///////////////////////////////////////////////////
// function to evaluate  the Bezier curve
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

vec4[16] gregory_to_bernstein(float u, float v, vec4[20] gregoryPoints)
{
    vec4 bernsteinPoints[16];
    const float eps = 0.0001;

    bernsteinPoints[0] = gregoryPoints[0];
    bernsteinPoints[1] = gregoryPoints[4];
    bernsteinPoints[2] = gregoryPoints[10];
    bernsteinPoints[3] = gregoryPoints[16];
    bernsteinPoints[4] = gregoryPoints[1];
    bernsteinPoints[5] =((1 - u) * gregoryPoints[13] + (1 - v) * gregoryPoints[14]) / (2 - u - v + eps);
    bernsteinPoints[6] = (u * gregoryPoints[12] + (1 - v) * gregoryPoints[11]) / (1 + u - v + eps);
    bernsteinPoints[7] = gregoryPoints[17];
    bernsteinPoints[8] = gregoryPoints[2];
    bernsteinPoints[9] = ((1 - u) * gregoryPoints[7] + v * gregoryPoints[8]) / (1 - u + v + eps);
    bernsteinPoints[10] =  (u * gregoryPoints[6] + v * gregoryPoints[5]) / (u + v + eps);
    bernsteinPoints[11] = gregoryPoints[18];
    bernsteinPoints[12] = gregoryPoints[3];
    bernsteinPoints[13] = gregoryPoints[9];
    bernsteinPoints[14] = gregoryPoints[15];
    bernsteinPoints[15] = gregoryPoints[19];

    return bernsteinPoints;
}

void main()
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 midValue[4];
    vec4 controlPoints[4];
    vec4 inData[20];
    for (int i = 0; i < 20; ++i)
    inData[i] = gl_in[i].gl_Position;

    vec4[16] bernsteinPoints = gregory_to_bernstein(u, v, inData);

    /*vec4 pos = mix(
    mix(bernsteinPoints[5], bernsteinPoints[6], u),
    mix(bernsteinPoints[9], bernsteinPoints[10], u),
    v);*/

    controlPoints = loadControlPoints(0, bernsteinPoints);
    midValue[0] = bezier(u, controlPoints);
    controlPoints = loadControlPoints(4, bernsteinPoints);
    midValue[1] = bezier(u, controlPoints);
    controlPoints = loadControlPoints(8, bernsteinPoints);
    midValue[2] = bezier(u, controlPoints);
    controlPoints = loadControlPoints(12, bernsteinPoints);
    midValue[3] = bezier(u, controlPoints);

    vec4 pos = bezier(v, midValue);

    gl_Position = u_MVP.Projection * u_MVP.View * u_MVP.Model * pos;
}
