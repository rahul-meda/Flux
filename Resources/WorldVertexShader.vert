#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertNormal;
layout (location = 2) in vec3 vertTangent;
layout (location = 3) in vec2 texCoord;

out vec2 fragTexCoord;
out vec3 fragPosT;	// tangent space
out vec3 eyePosT;
out vec3 lightPosT[5];

uniform vec3 lightPos[5];
uniform vec3 eyePos;
uniform mat4 MVP;
uniform mat4 M;

void main()
{
    gl_Position = MVP * vec4(vertPos, 1.0f);

	vec3 N = normalize(vec3(M * vec4(vertNormal, 0.0f)));
	vec3 T = vec3(M * vec4(vertTangent, 0.0f));
	// re-orthogonalize T with respect to N
	T = normalize(T - dot(T, N) * N);
	vec3 B = normalize(cross(N, T));
	mat3 TBN = transpose(mat3(T, B, N));

	eyePosT = TBN * eyePos;
	for(int i = 0; i < 5; ++i)
	{
		lightPosT[i] = TBN * lightPos[i];
	}
	vec3 fragPos = vec3(M * vec4(vertPos, 1.0f));
	fragPosT = TBN * fragPos;
	fragTexCoord = vec2(texCoord.x, texCoord.y);
}