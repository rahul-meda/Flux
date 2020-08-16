#version 330 core

out vec4 fragColor;

in vec2 fragTexCoord;

uniform sampler2D diffuseTexture;

void main()
{
    fragColor = texture(diffuseTexture, fragTexCoord);
}