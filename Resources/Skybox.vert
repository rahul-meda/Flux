#version 330 core

layout (location = 0) in vec3 vertPos;

out vec3 fragTexCoord;

uniform mat4 VP;

void main()
{
    fragTexCoord = vertPos;
	vec4 pos = VP * vec4(vertPos, 1.0f);
    gl_Position = pos.xyww;
}