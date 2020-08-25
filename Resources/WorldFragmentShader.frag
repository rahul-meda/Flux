#version 330 core

out vec4 fragColor;

in vec2 fragTexCoords;
in vec3 fragPos;
in vec3 normal;

uniform vec3 eyePos;
uniform vec3 lightPos[5];

// materials
uniform sampler2D albedoMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D glossMap;
uniform sampler2D occlusionMap;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

vec3 lightColors[5];

const float PI = 3.14159265359f;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap, fragTexCoords).rgb * 2.0f - 1.0f;

    vec3 Q1  = dFdx(fragPos);
    vec3 Q2  = dFdy(fragPos);
    vec2 st1 = dFdx(fragTexCoords);
    vec2 st2 = dFdy(fragTexCoords);

    vec3 N   = normalize(normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0f);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0f) + 1.0f);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0f);
    float k = (r*r) / 8.0f;

    float nom   = NdotV;
    float denom = NdotV * (1.0f - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0f);
    float NdotL = max(dot(N, L), 0.0f);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0f - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0f);	// apparently (1 - cos0) can be negative! (numerically)
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(max(1.0f - cosTheta, 0.0f), 5.0);
} 

void main()
{		
    vec3 albedo     = pow(texture(albedoMap, fragTexCoords).rgb, vec3(2.2f));
    float metallic  = texture(specularMap, fragTexCoords).r;
    float roughness = texture(glossMap, fragTexCoords).r;
    float ao        = 1.0f; // texture(occlusionMap, fragTexCoords).r;
	lightColors[0] = vec3(300.0f);
	lightColors[1] = vec3(300.0f);
	lightColors[2] = vec3(300.0f);
	lightColors[3] = vec3(300.0f);
	lightColors[4] = vec3(300.0f);

    vec3 N = getNormalFromMap();
    vec3 V = normalize(eyePos - fragPos);
	vec3 R = reflect(-V, N);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04f); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 5; ++i) 
    {
        // calculate per-light radiance
		vec3 ray = lightPos[i] - fragPos;
		float dist = length(ray);
        vec3 L = ray / dist;
        vec3 H = normalize(V + L);
        float attenuation = 1.0f / (dist * dist);
        vec3 radiance = lightColors[i] * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0f), F0);
           
        vec3 nominator    = NDF * G * F; 
        float denominator = 4.0f * max(dot(N, V), 0.0f) * max(dot(N, L), 0.0f) + 0.001f; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // energy conservation
        vec3 kD = vec3(1.0f) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0f - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0f);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;
    }   

    // ambient lighting
    //vec3 ambient = vec3(0.03f) * albedo * ao;
	vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
	vec3 kS = F;
    vec3 kD = vec3(1.0f) - kS;
    kD *= 1.0f - metallic;	  
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;

	// sample both the pre-filter map and the BRDF lut and combine them together 
	// as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0f;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec2 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0f));
    // gamma correction
    color = pow(color, vec3(1.0f/2.2f)); 

    fragColor = vec4(color, 1.0f);
}