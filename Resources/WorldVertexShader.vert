#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertTangent;
layout (location = 3) in vec3 vertBiTangent;// not needed?
layout (location = 4) in vec2 texCoords;

out vec2 fragTexCoords;
out vec3 fragPos;
out vec3 normal;
out vec3 tangent;
out vec3 biTangent;

uniform mat4 MVP;
uniform mat4 M;

void main()
{
    gl_Position = MVP * vec4(vertPos, 1.0f);

	normal = vec3(M * vec4(vertNormal, 0.0f));
	tangent = vec3(M * vec4(vertTangent, 0.0f));
	biTangent = vec3(M * vec4(vertBiTangent, 0.0f));
	fragPos = vec3(M * vec4(vertPos, 1.0f));
	fragTexCoords = texCoords * 40.0f;
}