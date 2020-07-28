#version 330 core

#define MAX_WEIGHTS 6
#define MAX_BONES 100

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in float boneWeigths[MAX_WEIGHTS];
layout (location = 9) in int boneIDs[MAX_WEIGHTS];

out vec3 vNormal;
out vec3 fragPos;
out vec2 fragTexCoord;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 boneTransforms[MAX_BONES];

void main()
{
	mat4 boneTx = mat4(0.0f);
	for(int i = 0; i < MAX_WEIGHTS; ++i)
	{
		boneTx += boneTransforms[boneIDs[i]] * boneWeigths[i];
	}
	vec4 pos = boneTx * vec4(vertPos, 1.0f);
    gl_Position = MVP * pos;
	vec4 normal = boneTx * vec4(vertNormal, 0.0f);
	vNormal = normalize(vec3(M * normal));
	fragPos = vec3(M * pos);
	fragTexCoord = vec2(texCoord.x, texCoord.y);
}