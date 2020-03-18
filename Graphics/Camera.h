#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Body;

class Camera
{
public:
	enum DIR {FWD, REV, RIGHT, LEFT, UP, DOWN, IDLE};

	Camera();

	Camera(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up);

	glm::vec3 position;
	glm::vec3 velocity;

	glm::vec3 fwd;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 target;

	bool follow;
	Body* body;

	void Translate(DIR mode);

	void Rotate(float yaw, float pitch);

	glm::mat4 ViewSpace();
};