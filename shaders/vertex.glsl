#version 330 core
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 fragNormal;
out vec3 fragPosition;

void main()
{
    vec4 worldPosition = model * vec4(inPosition, 1.0);
    fragPosition = worldPosition.xyz;
    fragNormal = mat3(transpose(inverse(model))) * inNormal;
    gl_Position = projection * view * worldPosition;
}
