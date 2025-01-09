#version 330 core
in vec2 fragUV;
in vec3 fragNormal;
in vec3 fragPosition;
out vec4 FragColor;

uniform vec3 lightPos;              // Position of the light source
uniform vec3 viewPos;               // Position of the camera
uniform sampler2D colorTexture;     // Color texture
uniform bool useColorTexture;
uniform sampler2D normalTexture;     // Normal texture
uniform bool useNormalTexture;

void main()
{
    // Normalize the normal
    vec3 normal = normalize(fragNormal);
    
    // Compute the direction of the light source
    vec3 lightDir = normalize(lightPos - fragPosition);
    
    // Diffuse component of the lambertian reflection model
    float diff = max(dot(normal, lightDir), 0.0);

    // Base color of the object from the color texture
    vec3 baseColor;
    if(useColorTexture)
    {
        baseColor = texture(colorTexture, fragUV).rgb;
    }
    else
    {
        baseColor = vec3(1.0, 1.0, 1.0); // default white color
    }
    vec3 diffuse = diff * baseColor;
    
    // Add some ambient lighting (optional)
    vec3 ambient = 0.1 * baseColor;
    
    FragColor = vec4(ambient + diffuse, 1.0);
}
