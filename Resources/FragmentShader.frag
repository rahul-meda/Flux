#version 330 core

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 outColor;

uniform sampler2D woodTexture;
uniform sampler2D smileyTexture;

void main()
{
    outColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);//mix(texture(woodTexture, fragTexCoord), texture(smileyTexture, fragTexCoord), 0.2f);
}