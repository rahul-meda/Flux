#version 330 core

#define nPointLights 5

in vec3 vNormal;
in vec3 fragPos;	// fragment world position
in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D emissionTexture;
uniform vec3 lightMap;	// strengths of lighting maps

uniform vec3 lightPos[nPointLights];
uniform vec3 eyePos;
uniform float time;

vec3 normal;
vec3 viewDir;
vec3 globalLightDir;
vec3 lightColors[5];
vec4 txtColor;

vec3 CalcGlobalLight();
vec3 CalcPointLight(int index);
 
void main()
{
	normal = normalize(vNormal);
	viewDir = normalize(eyePos - fragPos);
	globalLightDir = normalize(vec3(-1.0f, -1.0f, -1.0f));

	lightColors[0] = vec3(1.0f, 0.0f, 0.0f);
	lightColors[1] = vec3(0.0f, 1.0f, 0.0f);
	lightColors[2] = vec3(0.0f, 0.0f, 1.0f);
	lightColors[3] = vec3(1.0f, 1.0f, 0.0f);
	lightColors[4] = vec3(1.0f);

	float ambientStr = 0.2f;
	txtColor = texture(diffuseTexture, fragTexCoord);
	vec3 ambient = ambientStr * txtColor.rgb;

	vec3 result = ambient;
	
	result += CalcGlobalLight();

	float sin_time = sin(time); 

	for(int i = 0; i < nPointLights - 1; ++i)
	{
		result += CalcPointLight(i) * lightColors[i];// * abs(sin_time);
	}
	
	result += CalcPointLight(nPointLights - 1) * lightColors[nPointLights - 1];

	txtColor = texture(emissionTexture, fragTexCoord);
	vec3 emission = 0.7f * lightMap.z * txtColor.rgb;// + glm::vec2(0.0f, time/10.0f)).rgb;

	result += emission;

    outColor = vec4(result, 1.0f);
}

vec3 CalcGlobalLight()
{
	txtColor = texture(diffuseTexture, fragTexCoord);
	vec3 diffuse = 0.5f * max(dot(-globalLightDir, normal), 0.0f) * txtColor.rgb;

	float specularStr = 0.25f;
	int shininess = 64;
	vec3 refDir = normalize(reflect(globalLightDir, normal));
	float spec = pow(max(dot(viewDir, refDir), 0.0), shininess);
	vec3 specular = specularStr * spec * txtColor.rgb;

	txtColor = texture(specularTexture, fragTexCoord);
	specular += 0.5f * lightMap.y * spec * txtColor.rgb;

	return (diffuse + specular);
}

vec3 CalcPointLight(int index)
{
	vec3 lightDir = normalize(fragPos - lightPos[index]);
	txtColor = texture(diffuseTexture, fragTexCoord);
	
	vec3 diffuse = max(dot(-lightDir, normal), 0.0f) * txtColor.rgb;

	float specularStr = 0.25f;
	int shininess = 64;
	vec3 refDir = normalize(reflect(lightDir, normal));
	float spec = pow(max(dot(viewDir, refDir), 0.0), shininess);
	vec3 specular = specularStr * spec * txtColor.rgb;

	txtColor = texture(specularTexture, fragTexCoord);
	specular += 0.5f * lightMap.y * spec * txtColor.rgb;

	// attenuation
	float Kc = 1.0f;
	float Kl = 0.022f; //0.014;//0.022f;
	float Kq = 0.0019f; //0.0007;//0.0019f;
	float dist = length(fragPos - lightPos[index]);
	float dist2 = dist * dist;
	float attenuation = 1.0f / (Kc + Kl * dist + Kq * dist2);

	return (diffuse + specular) * attenuation;
}