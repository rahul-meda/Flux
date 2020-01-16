#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Transform
{
public:
	Transform(glm::vec3 pos, glm::quat orient = glm::identity<glm::quat>(), glm::vec3 scale = glm::vec3(1.0))
		: position(pos), orientation(orient), scale(scale)
	{}

	Transform() {}

	glm::vec3 position;
	glm::quat orientation;
	glm::vec3 scale;
};