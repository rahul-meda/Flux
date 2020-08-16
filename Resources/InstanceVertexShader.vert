#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in mat4 M;

out vec3 vNormal;
out vec3 fragPos;
out vec2 fragTexCoord;

uniform mat4 VP;

void main()
{
    gl_Position = VP * M * vec4(vertPos, 1.0f); 
	vNormal = normalize(vec3(M * vec4(vertNormal, 0.0f)));
	fragPos = vec3(M * vec4(vertPos, 1.0f));
	fragTexCoord = texCoord;
}