#version 440 core

layout (vertices = 16) out;

uniform int u_UDensity;
uniform int u_VDensity;

void
main()
{
    int du = u_UDensity;
    int dv = u_VDensity;

    gl_TessLevelOuter[0] = du;//u
    gl_TessLevelOuter[1] = dv;//v
    gl_TessLevelOuter[2] = du;//u
    gl_TessLevelOuter[3] = dv;//v
    gl_TessLevelInner[0] = dv;
    gl_TessLevelInner[1] = du;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}