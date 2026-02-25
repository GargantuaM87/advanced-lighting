#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vTexCoords;
} fs_in;

uniform samplerCube depthMap;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objColor;

uniform float far_plane;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

float ShadowCalculation(vec3 fragPos, float bias);

void main() 
{
    vec3 color = objColor;
    vec3 normal = normalize(fs_in.vNormal);
    vec3 lightColor = vec3(1.0);
    // ambient
    vec3 ambient = 0.15 * lightColor;
    // diffuse
    vec3 lightDir =  normalize(lightPos - fs_in.vFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.vFragPos);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = spec * lightColor;
    // calculate shadows
    float bias = 0.15;
    float shadow = ShadowCalculation(fs_in.vFragPos, bias);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;

    FragColor = vec4(lighting, 1.0);
}

float ShadowCalculation(vec3 fragPos, float bias)
{
    // vector between fragment and light positions
    vec3 fragToLight = fragPos - lightPos;
    // get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    // test for shadows
    float shadow = 0.0;
    float samples = 20.0;
    float viewDist = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDist / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i) {
        float closestDepth = texture(depthMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
        closestDepth *= far_plane;
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }

    shadow /= float(samples);
    return shadow;
}