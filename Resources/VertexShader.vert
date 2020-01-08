#version 330 core

layout (location = 0) in vec3 vertPos;
layout (location = 1) in vec3 vertColor;
layout (location = 2) in vec2 texCoord;

out vec3 fragColor;
out vec2 fragTexCoord;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vec4(vertPos, 1.0f);
	fragColor = vertColor;
	fragTexCoord = vec2(texCoord.x, texCoord.y);
}