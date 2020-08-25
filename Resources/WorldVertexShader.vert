#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertTangent;
layout (location = 3) in vec3 vertBiTangent;// not needed?
layout (location = 4) in vec2 texCoords;

out vec2 fragTexCoords;
out vec3 fragPos;
out vec3 normal;

uniform mat4 MVP;
uniform mat4 M;

void main()
{
    gl_Position = MVP * vec4(vertPos, 1.0f);

	normal = vec3(M * vec4(vertNormal, 0.0f));
	fragPos = vec3(M * vec4(vertPos, 1.0f));
	fragTexCoords = texCoords;
}