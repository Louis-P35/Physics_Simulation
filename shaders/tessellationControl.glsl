#version 400 core
layout(vertices = 3) out;  // Handle triangles

in vec3 fragNormal[];     // Input from vertex shader
in vec3 fragPosition[];   // Input from vertex shader
in vec2 fragUV[];         // Input from vertex shader
in mat3 TBN[];            // Input from vertex shader

out vec3 tcsNormal[];     // Output to tessellation evaluation shader
out vec3 tcsPosition[];
out vec2 tcsUV[];
out mat3 tcsTBN[];        // Pass TBN if needed

uniform float tessellationFactor;  // Subdivision factor

void main()
{
    // Pass attributes to the TES
    tcsNormal[gl_InvocationID] = fragNormal[gl_InvocationID];
    tcsPosition[gl_InvocationID] = fragPosition[gl_InvocationID];
    tcsUV[gl_InvocationID] = fragUV[gl_InvocationID];
    tcsTBN[gl_InvocationID] = TBN[gl_InvocationID];

    // Give the position and others attributs to the TES
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // Fiwed tessellation level
    gl_TessLevelOuter[0] = tessellationFactor;
    gl_TessLevelOuter[1] = tessellationFactor;
    gl_TessLevelOuter[2] = tessellationFactor;
    gl_TessLevelInner[0] = tessellationFactor;
}
