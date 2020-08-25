#version 330 core

layout (location = 0) in vec3 vertPos;

out vec3 fragPos;

uniform mat4 P;
uniform mat4 V;

void main()
{
    fragPos = vertPos;  
    gl_Position =  P * V * vec4(fragPos, 1.0f);
}