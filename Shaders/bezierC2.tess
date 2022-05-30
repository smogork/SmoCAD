#version 440 core

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38

layout (vertices = 4) out;

struct MVP
{
    mat4 Model;
    mat4 View;
    mat4 Projection;
};

uniform MVP u_MVP;
uniform vec2 u_viewportSize;

//https://www.khronos.org/opengl/wiki/Tessellation_Control_Shader/Defined_Inputs
int countTesselationChunks()
{
    float maxX = FLT_MIN, maxY = FLT_MIN, minX = FLT_MAX, minY = FLT_MAX;
    for (int i = 0; i < gl_PatchVerticesIn; ++i)
    {
        vec4 NDCPoint = u_MVP.Projection * u_MVP.View * u_MVP.Model * gl_in[i].gl_Position;
        NDCPoint /= NDCPoint.w;
        vec2 screenPoint = vec2(
        clamp((NDCPoint.x + 1) * (u_viewportSize.x / 2), 0, u_viewportSize.x),
        clamp((NDCPoint.y + 1) * (u_viewportSize.y / 2), 0, u_viewportSize.y)
        );

        maxX = max(maxX, screenPoint.x);
        maxY = max(maxY, screenPoint.y);
        minX = min(minX, screenPoint.x);
        minY = min(minY, screenPoint.y);
    }

    int length =  int(max(maxX - minX, maxY - minY));
    length /= 4;
    return min(max(4, length), 64);
}

void
main()
{
    gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = countTesselationChunks();

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}