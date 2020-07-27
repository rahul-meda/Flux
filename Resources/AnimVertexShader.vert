#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec4 boneWeigths;
layout (location = 4) in ivec4 boneIDs;

out vec3 vNormal;
out vec3 fragPos;
out vec2 fragTexCoord;

const int MAX_BONES = 100;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 boneTransforms[MAX_BONES];

void main()
{
	mat4 boneTx = boneTransforms[boneIDs[0]] * boneWeigths[0];
				+ boneTransforms[boneIDs[1]] * boneWeigths[1];
				+ boneTransforms[boneIDs[2]] * boneWeigths[2];
				+ boneTransforms[boneIDs[3]] * boneWeigths[3];
				boneTx = mat4(1.0f);
	vec4 pos = boneTx * vec4(vertPos, 1.0f);
    gl_Position = MVP * pos;
	vec4 normal = boneTx * vec4(vertNormal, 0.0f);
	vNormal = normalize(vec3(M * normal));
	fragPos = vec3(M * pos);
	fragTexCoord = vec2(texCoord.x, texCoord.y);
}