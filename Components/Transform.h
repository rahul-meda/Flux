#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform
{
public:
	Transform(glm::vec3 pos, glm::quat q = glm::identity<glm::quat>())
		: position(pos)
	{
		R = glm::toMat3(q);
	}

	Transform() {}

	glm::vec3 position;
	glm::mat3 R;

	glm::mat4 CombinedTransform() const
	{
		glm::mat4 result = glm::translate(glm::mat4(1.0f), position);
		result *= glm::mat4(R);
		return result;
	}
};

#include "Transform.inl"