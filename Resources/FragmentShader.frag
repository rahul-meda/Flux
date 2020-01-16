#version 330 core

in vec3 vNormal;
in vec3 vPosW;
in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D woodTexture;
uniform sampler2D smileyTexture;

uniform vec3 objColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 eyePos;

void main()
{
	float ambientStr = 0.3f;
	vec3 ambient = ambientStr * lightColor;

	vec3 lightDir = normalize(vPosW - lightPos);
	vec3 diffuse = max(dot(-lightDir, vNormal), 0.0f) * lightColor;

	float specularStr = 0.5f;
	int shininess = 32;
	vec3 viewDir = normalize(eyePos - vPosW);
	vec3 refDir = normalize(reflect(lightDir, vNormal));
	float spec = pow(max(dot(viewDir, refDir), 0.0), shininess);
	vec3 specular = specularStr * spec * lightColor;

	vec3 result = (ambient + diffuse + specular) * objColor;
    outColor = vec4(result, 1.0f);

	//mix(texture(woodTexture, fragTexCoord), texture(smileyTexture, fragTexCoord), 0.2f);
}