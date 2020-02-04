#pragma once

#include <glm/glm.hpp>

void ComputeBasis(const glm::vec3& v1, glm::vec3* v2, glm::vec3* v3)
{
	// At least one component of a unit vector must be greater than 0.57735

	if (glm::abs(v1.x) >= 0.57735)
		*v2 = glm::vec3(v1.y, -v1.x, 0.0f);
	else
		*v2 = glm::vec3(0.0f, v1.z, -v1.y);

	*v2 = glm::normalize(*v2);

	*v3 = glm::normalize(glm::cross(v1, *v2));
}