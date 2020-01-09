#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#define CAM_SPEED 3.0f

class Camera
{
public:
	enum DIR {FWD, REV, RIGHT, LEFT, UP, DOWN};

	Camera();

	Camera(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up);

	glm::vec3 position;

	glm::vec3 fwd;

	glm::vec3 up;

	glm::mat4 ViewSpace();

	void Translate(DIR mode, double dt);

	void Rotate(float yaw, float pitch);
};