#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vTexCoords;
    vec4 vFragPosLightSpace;
} fs_in;

uniform sampler2D texture_diffuse1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace);

void main() 
{
    vec3 color = texture(texture_diffuse1, fs_in.vTexCoords).rgb;
    vec3 normal = normalize(fs_in.vNormal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient =  0.15 * lightColor;
    // diffuse
    vec3 lightDir =  normalize(lightPos - fs_in.vFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.vFragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;
    // calculate shadows
    float shadow = ShadowCalculation(fs_in.vFragPosLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular) * color);

    FragColor = vec4(lighting, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    //perform perspective division (divide by W_c in clip space)
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform range from [-1, 1] to [0, 1]
    projCoords = projCoords * 0.5 + 0.5;
    // closest depth value from light's perspective in the range [0, 1]
    float closestDepth =  texture(shadowMap, projCoords.xy).r;
    // depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check wheter current frag pos is in shadow
    float shadow = currentDepth > closestDepth ? 1.0 : 0.0;

    return shadow;
}