
#include "Camera.h"
#include "../Components/Body.h"

#define CAM_SPEED 100.0f

Camera::Camera()
{}

Camera::Camera(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up)
	: position(pos), velocity(0.0f), fwd(fwd), up(up), follow(false)
{
	target = position + fwd;
}

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

	velocity += 10.0f * dt * dir;

	position += velocity * dt;

	target = position + fwd;
}

void Camera::Rotate(float yaw, float pitch)
{
	fwd.x = cos(glm::radians(yaw - 90.0f)) * cos(glm::radians(pitch));
	fwd.y = sin(glm::radians(pitch));
	fwd.z = sin(glm::radians(yaw - 90.0f)) * cos(glm::radians(pitch));

	fwd = glm::normalize(fwd);
	right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), fwd));
	up = glm::cross(fwd, right);

	target = position + fwd;
}

void Camera::Track(TRACK mode)
{
	glm::vec3 dir;

	switch (mode)
	{
	case LINE:
		dir = fwd;
		break;
	}
}

glm::mat4 Camera::ViewSpace()
{
	return glm::lookAt(position, target, up);
}