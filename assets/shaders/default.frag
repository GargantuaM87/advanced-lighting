#version 330 core
out vec4 FragColor;

struct Material {
    vec3 objectColor;
    float shininess;
};

struct DirectionalLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
}; 
struct SpotLight 
{
    vec3 position;
    vec3 direction;

    float innerCutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform PointLight u_pointLights; 
uniform DirectionalLight u_dirLight; 
uniform SpotLight u_spotLight;
uniform Material u_mat;
uniform vec3 u_viewPos;

in vec3 FragPos;
in vec3 Normal;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
   vec3 norm = normalize(Normal);
   vec3 viewDir = normalize(u_viewPos - FragPos);
   
   
   vec3 result = CalcDirLight(u_dirLight, norm, viewDir);
   result += CalcPointLight(u_pointLights, norm, FragPos, viewDir);
   
  
   //result += CalcSpotLight(u_spotLight, norm, FragPos, viewDir);

   FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirectionalLight light, vec3 normal, vec3 viewDir) 
{
    vec3 lightDir = normalize(-light.direction);
    vec3 halfwayDir =  normalize(lightDir + viewDir);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), u_mat.shininess);
    // combine results
    vec3 ambient = light.ambient * u_mat.objectColor;
    vec3 diffuse = light.diffuse * diff * u_mat.objectColor;
    vec3 specular = light.specular * spec;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) 
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir =  normalize(lightDir + viewDir);

    //diffuse
    float diff = max(dot(normal, lightDir), 0.0);

    //specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), u_mat.shininess);

    //attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1 / (light.constant + light.linear * distance + light.quadratic * (distance * distance)); 

    // combine results
    vec3 ambient = light.ambient * u_mat.objectColor;
    vec3 diffuse = light.diffuse * diff * u_mat.objectColor;
    vec3 specular = light.specular * spec;

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    vec3 halfwayDir =  normalize(lightDir + viewDir);
    
    // ambient lighting
    vec3 ambient = light.ambient * u_mat.objectColor;
    
    // diffuse lighting
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * u_mat.objectColor;
    
    // specular lighting
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, halfwayDir), 0.0), u_mat.shininess);
    vec3 specular = light.specular * spec;

    // soft-shadow calculations
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.innerCutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse *= intensity;
    specular *= intensity;

    //attenuation 
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return (ambient + diffuse + specular);
}