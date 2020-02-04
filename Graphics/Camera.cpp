
#include "Camera.h"

#define CAM_SPEED 7.0f

Camera::Camera()
{}

Camera::Camera(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up)
	: position(pos), fwd(fwd), up(up)
{}

void Camera::Translate(DIR mode)
{
	glm::vec3 dir, right;

	right = glm::normalize(glm::cross(fwd, up));

	switch (mode)
	{
	case FWD:
		dir = fwd;
		break;
	case REV:
		dir = -fwd;
		break;
	case RIGHT:
		dir = right;
		break;
	case LEFT:
		dir = -right;
		break;
	case UP:
		dir = up;
		break;
	case DOWN:
		dir = -up;
		break;
	}

	const static float dt = 1.0f / 60.0f;
	position += CAM_SPEED * dir * dt;
}

void Camera::Rotate(float yaw, float pitch)
{
	fwd.x = cos(glm::radians(yaw - 90.0f)) * cos(glm::radians(pitch));
	fwd.y = sin(glm::radians(pitch));
	fwd.z = sin(glm::radians(yaw - 90.0f)) * cos(glm::radians(pitch));

	fwd = glm::normalize(fwd);
}

glm::mat4 Camera::ViewSpace()
{
	return glm::lookAt(position, position + fwd, up);
}