
#include <iostream>
#include "Camera.h"
#include "../Components/Body.h"
#include <glm/gtx/euler_angles.hpp>

#define CAM_SPEED 100.0f
#define deltaT (1.0f/60.0f)
#define PI 3.14159265359f

Camera::Camera()
{}

Camera::Camera(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up)
	: position(pos), velocity(0.0f), fwd(fwd), up(up), follow(false)
{
	right = glm::normalize(glm::cross(fwd, up));
	target = position + fwd;
	glm::mat3 R;
	R[0] = right;
	R[1] = up;
	R[2] = fwd;
	orient = glm::quat_cast(R);
	pitch = 135.0f;
	yawOff = 0.0f;
	isIdle = true;
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

	velocity += 10.0f * deltaT * dir;

	position += velocity * deltaT;

	target = position + fwd;
}

void Camera::Rotate(float yaw, float pitch, float dx, float dy)
{
	float y = glm::radians(yaw);
	float p = glm::radians(pitch);
	float cy = cos(y);
	float cp = cos(p);
	float sy = sin(y);
	float sp = sin(p);

	fwd.x = cy * cp;
	fwd.y = sp;
	fwd.z = sy * cp;
	right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), fwd);
	up = glm::cross(fwd, right);

	target = position + fwd;
}

void Camera::Follow(const float dx, const float dy)
{
	float dx2 = dx * dx;
	float dy2 = dy * dy;

	if (!isIdle)
	{
		if (dx2 > dy2)
		{
			yawOff -= dx;
		}
		else
		{
			pitch += dy;
			pitch = glm::clamp(pitch, 95.0f, 175.0f);
		}
	}

	float phi = PI - glm::radians(pitch);
	float l = radius * cosf(phi);
	float h = radius * sinf(phi);
	Transform tx = body->GetTransform();
	float theta = acosf(tx.R[0].x) + glm::radians(yawOff);
	float offX = l * sinf(theta);
	float offZ = l * cosf(theta);

	target = tx.position + offset;
	position.x = target.x - offX;
	position.z = target.z - offZ;
	position.y = target.y + h;

	fwd = target - position;
	right = glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), fwd);
	up = glm::cross(fwd, right);
}

void Camera::RotateSpherical(float yaw, float pitch, glm::vec3 center, float radius)
{
	float sin_t = sinf(glm::radians(yaw));
	float cos_t = cosf(glm::radians(yaw));
	float sin_p = sinf(glm::radians(pitch + 90.0f));
	float cos_p = cosf(glm::radians(pitch + 90.0f));

	position.x = radius * cos_t * sin_p;
	position.y = radius * sin_t * sin_p;
	position.z = radius * cos_p;
	position += center;

	target = center;
	fwd = glm::normalize(center - position);
	right = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), fwd));
	up = glm::normalize(glm::cross(fwd, right));
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