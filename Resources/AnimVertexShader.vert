#version 330 core

#define MAX_WEIGHTS 6
#define MAX_BONES 100

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertTangent;
layout (location = 3) in vec3 vertBiTangent; // not needed 
layout (location = 4) in vec3 boneWeigths1;
layout (location = 5) in vec3 boneWeigths2;
layout (location = 6) in ivec3 boneIDs1;
layout (location = 7) in ivec3 boneIDs2;
layout (location = 8) in vec2 texCoords;

out vec2 fragTexCoords;
out vec3 fragPos;
out vec3 normal;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 boneTransforms[MAX_BONES];

void main()
{
	mat4 boneTx = mat4(0.0f);
	for(int i = 0; i < 3; ++i)
	{
		boneTx += boneTransforms[boneIDs1[i]] * boneWeigths1[i];
	}
	for(int i = 0; i < 3; ++i)
	{
		boneTx += boneTransforms[boneIDs2[i]] * boneWeigths2[i];
	}
	vec4 pos = boneTx * vec4(vertPos, 1.0f);
    gl_Position = MVP * pos;

	normal = vec3(M * boneTx * vec4(vertNormal, 0.0f));
	fragPos = vec3(M * pos);
	fragTexCoords = texCoords;
}