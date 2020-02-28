#pragma once

#include <glm/glm.hpp>

static void ComputeBasis(const glm::vec3& v1, glm::vec3* v2, glm::vec3* v3)
{
	// At least one component of a unit vector must be greater than 0.57735

	if (glm::abs(v1.x) >= 0.57735)
		*v2 = glm::vec3(v1.y, -v1.x, 0.0f);
	else
		*v2 = glm::vec3(0.0f, v1.z, -v1.y);

	*v2 = glm::normalize(*v2);

	*v3 = glm::normalize(glm::cross(v1, *v2));
}

// quaternion equivalent matrix product - ref: Marijn Tamis
static void QuatSkewLeft(const glm::quat& q, glm::mat4x4& m)
{
	m[0] = glm::vec4(q.w, q.x, q.y, q.z);
	m[1] = glm::vec4(-q.x, q.w, q.z, -q.y);
	m[2] = glm::vec4(-q.y, -q.z, q.w, q.x);
	m[3] = glm::vec4(-q.z, q.y, -q.x, q.w);
}

static void QuatSkewRight(const glm::quat& q, glm::mat4x4& m)
{
	m[0] = glm::vec4(q.w, q.x, q.y, q.z);
	m[1] = glm::vec4(-q.x, q.w, -q.z, q.y);
	m[2] = glm::vec4(-q.y, q.z, q.w, -q.x);
	m[3] = glm::vec4(-q.z, -q.y, q.x, q.w);
}