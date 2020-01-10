#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 texCoord;

out vec3 vNormal;
out vec3 vPosW;
out vec2 fragTexCoord;

uniform mat4 MVP;
uniform mat4 M;

void main()
{
    gl_Position = MVP * vec4(vertPos, 1.0f);
	vNormal = normalize(vec3(M * vec4(vertNormal, 0.0f)));
	vPosW = vec3(M * vec4(vertPos, 1.0f));
	fragTexCoord = vec2(texCoord.x, texCoord.y);
}