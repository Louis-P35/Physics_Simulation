#version 330 core
in vec2 tessUV;
in vec3 tessNormal;
in vec3 tessPosition;
in mat3 tessTBN;
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
    vec3 normal = normalize(tessNormal);

    // Apply normal mapping if activated
    if(useNormalTexture)
    {
        // Get the normal from the normal map
        vec3 tangentNormal = texture(normalTexture, tessUV).rgb;
        // Remap it's coordinates from [0,1] to [-1,1]
        // Because texture store RGB data between [0, 1] but normal vectors are between the [-1, 1] range
        tangentNormal = tangentNormal * 2.0 - 1.0;
        // Transform the normale from tangent space to world/camera space with the TBN matrix
        normal = normalize(tessTBN * tangentNormal);
    }
    
    // Compute the direction of the light source
    vec3 lightDir = normalize(lightPos - tessPosition);
    
    // Diffuse component of the lambertian reflection model
    float diff = max(dot(normal, lightDir), 0.0);

    // Base color of the object from the color texture
    vec3 baseColor;
    if(useColorTexture)
    {
        baseColor = texture(colorTexture, tessUV).rgb;
    }
    else
    {
        baseColor = vec3(1.0, 1.0, 1.0); // default white color
    }
    vec3 diffuse = diff * baseColor;
    
    // Add some ambient lighting (optional)
    vec3 ambient = 0.1 * baseColor;
    
    FragColor = vec4(ambient + diffuse, 1.0);
    //FragColor = vec4(normalize(normal) * 0.5 + 0.5, 1.0);
}
