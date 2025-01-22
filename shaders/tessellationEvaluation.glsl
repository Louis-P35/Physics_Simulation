#version 400 core
layout(triangles, equal_spacing, ccw) in;

uniform sampler2D bumpTexture;   // Bump map used for displacement
uniform bool useBumpTexture;
uniform float displacementScale;  // Displacementc scale factor
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


in vec3 tcsNormal[];      // Input from tessellation control shader
in vec3 tcsPosition[];    // Input from tessellation control shader
in vec2 tcsUV[];          // Input from tessellation control shader
in mat3 tcsTBN[];         // Input from tessellation control shader

out vec2 tessUV;
out vec3 tessNormal;
out vec3 tessPosition;
out mat3 tessTBN;

void main()
{
    // Interpolate TBN barycentrically
    tessTBN = tcsTBN[0] * gl_TessCoord.x +
              tcsTBN[1] * gl_TessCoord.y +
              tcsTBN[2] * gl_TessCoord.z;

    // Barycentric interpolation of UVs and others attributs
    tessUV = tcsUV[0] * gl_TessCoord.x +
             tcsUV[1] * gl_TessCoord.y +
             tcsUV[2] * gl_TessCoord.z;

    vec3 pos = tcsPosition[0] * gl_TessCoord.x +
               tcsPosition[1] * gl_TessCoord.y +
               tcsPosition[2] * gl_TessCoord.z;

    vec3 normal = tcsNormal[0] * gl_TessCoord.x +
                  tcsNormal[1] * gl_TessCoord.y +
                  tcsNormal[2] * gl_TessCoord.z;

    // Apply displacement mapping
    float displacement = 0.0;
    if (useBumpTexture)
    {
        displacement = texture(bumpTexture, tessUV).r * displacementScale;
    }
    
    pos += normalize(normal) * displacement;

    // Give the transformed attributs to the fragment shader
    tessPosition = pos;
    tessNormal = normalize(normal);
    gl_Position = projection * view * model * vec4(pos, 1.0);
}
