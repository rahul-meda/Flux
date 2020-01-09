#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform(glm::vec3 pos, glm::quat orient)
		: position(pos), orientation(orient)
	{}

	Transform() {}

	glm::vec3 position;
	glm::quat orientation;
};