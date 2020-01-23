#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
	Transform(glm::vec3 pos, glm::quat q = glm::identity<glm::quat>(), glm::vec3 scale = glm::vec3(1.0))
		: position(pos), scale(scale)
	{
		R = glm::toMat3(q);
	}

	Transform() {}

	glm::vec3 position;
	glm::mat3 R;
	glm::vec3 scale;
};