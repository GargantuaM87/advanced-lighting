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
    sampler2D texture_height1;
};
uniform Material u_mat;


void main()
{
    // obtain normal from normal map in range [0, 1]
    vec3 norm = texture(u_mat.texture_normal1, TexCoords).rgb;
    // transform normal vector to range [-1, 1]
    norm = normalize(norm * 2.0 - 1.0); // normal in tangent space

    // texture color
    vec3 color = texture(u_mat.texture_diffuse1, TexCoords).rgb;
    // ambient
    vec3 ambient = 0.1 * color;
    // diffusement
    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);
    float diff = max(dot(lightDir, norm), 0.0);
    vec3 diffuse = diff * color;
    // specular
    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}