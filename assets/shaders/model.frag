#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;

struct Material {
    float shininess;
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_depth1;
};
uniform Material u_mat;

uniform float heightScale;

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);


void main()
{
    // offset texture coordinates with Parallax Mapping
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec2 texCoords = ParallaxMapping(TexCoords, viewDir);
    
    if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
        discard;

    // obtain normal from normal map in range [0, 1]
    vec3 norm = texture(u_mat.texture_normal1, texCoords).rgb;
    // transform normal vector to range [-1, 1]
    norm = normalize(norm * 2.0 - 1.0); // normal in tangent space

    // texture color
    vec3 color = texture(u_mat.texture_diffuse1, texCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffusement
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) 
{
    float height = texture(u_mat.texture_depth1, texCoords).r;
    vec2 p = viewDir.xy / viewDir.z * (height * heightScale);
    return texCoords - p;
}