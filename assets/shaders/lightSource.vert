#version 330 core
layout (location = 0) in vec3 aPos;

 // Uniforms are inputted from the CPU to all parallel threads on the GPU
uniform mat4 camMatrix;
uniform mat4 model;


void main()
{
    gl_Position = camMatrix * model * vec4(aPos, 1.0);
}