#version 330 core
in vec3 fragNormal;
in vec3 fragPosition;
out vec4 FragColor;

uniform vec3 lightPos;    // Position of the light source
uniform vec3 viewPos;     // Position of the camera

void main()
{
    // Normalize the normal
    vec3 normal = normalize(fragNormal);
    
    // Compute the direction of the light source
    vec3 lightDir = normalize(lightPos - fragPosition);
    
    // Diffuse component of the lambertian reflection model
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Base color of the object
    vec3 baseColor = vec3(1.0, 0.5, 0.31);  // par exemple
    vec3 diffuse = diff * baseColor;
    
    // Add some ambient lighting (optional)
    vec3 ambient = 0.1 * baseColor;
    
    FragColor = vec4(ambient + diffuse, 1.0);
}
