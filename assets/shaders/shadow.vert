#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec3 vFragPos;
    vec3 vNormal;
    vec2 vTexCoords;
    vec4 vFragPosLightSpace;
} vs_out;

uniform mat4 lightSpaceMatrix;
uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.vFragPos = vec3(model * vec4(aPos, 1.0));
    vs_out.vNormal = transpose(inverse(mat3(model))) * aNormal;
    vs_out.vTexCoords = aTexCoords;
    vs_out.vFragPosLightSpace = lightSpaceMatrix * vec4(vs_out.vFragPos, 1.0);
    //gl_Position = projection * view * vec4(vs_out.vFragPos, 1.0);
    gl_Position = projection * view * vec4(vs_out.vFragPos, 1.0);
}