#version 330 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inTangent;
layout(location = 4) in vec3 inBitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragNormal;
out vec3 fragPosition;
out vec2 fragUV;
out mat3 TBN;  // TBN matrix for normal mapping

void main()
{
    //gl_Position = vec4(inPosition, 1.0);
    vec4 worldPosition = model * vec4(inPosition, 1.0);
    fragPosition = worldPosition.xyz;
    fragNormal = mat3(transpose(inverse(model))) * inNormal;
    //fragNormal = inNormal;
    fragUV = inUV;

    // Transformation of tangent, bitangent and normal vectors in world space
    vec3 T = normalize(mat3(model) * inTangent);
    vec3 B = normalize(mat3(model) * inBitangent);
    vec3 N = normalize(mat3(model) * inNormal);

    // Construction of the TBN matrix
    TBN = mat3(T, B, N);

    // The projection * view will be handled in the tessellation evaluation shader
    //gl_Position = projection * view * worldPosition;
    gl_Position = worldPosition;
}
