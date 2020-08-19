#version 330 core

#define MAX_WEIGHTS 6
#define MAX_BONES 100

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertTangent;
layout (location = 3) in vec3 boneWeigths1;
layout (location = 4) in vec3 boneWeigths2;
layout (location = 5) in ivec3 boneIDs1;
layout (location = 6) in ivec3 boneIDs2;
layout (location = 7) in vec2 texCoord;

out vec2 fragTexCoord;
out vec3 fragPosT;	// tangent space
out vec3 eyePosT;
out vec3 lightPosT[5];

uniform vec3 lightPos[5];
uniform vec3 eyePos;
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

	vec4 normal = boneTx * vec4(vertNormal, 0.0f);
	vec3 N = normalize(vec3(M * normal));
	vec4 tangent = boneTx * vec4(vertTangent, 0.0f);
	vec3 T = normalize(vec3(M * tangent));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	eyePosT = TBN * eyePos;
	for(int i = 0; i < 5; ++i)
	{
		lightPosT[i] = TBN * lightPos[i];
	}
	fragPosT = TBN * vec3(M * pos);
	fragTexCoord = vec2(texCoord.x, texCoord.y);
}