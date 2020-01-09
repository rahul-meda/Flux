#version 330 core

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D woodTexture;
uniform sampler2D smileyTexture;

uniform vec3 objColor;
uniform vec3 lightColor;

void main()
{
    outColor = vec4(objColor * lightColor, 1.0f);//mix(texture(woodTexture, fragTexCoord), texture(smileyTexture, fragTexCoord), 0.2f);
}