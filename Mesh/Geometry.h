#pragma once

#include "../Components/Model.h"
#include <vector>

#define PI 3.14159265359f

struct HalfSpace
{
	glm::vec3 normal;
	float distance;

	HalfSpace();
	HalfSpace(const glm::vec3& normal, const float distance);
	HalfSpace(const glm::vec3& normal, const glm::vec3& point);

	glm::vec3 Origin() const;
	float Distance(const glm::vec3& point) const;
	glm::vec3 Projection(const glm::vec3& point) const;

	bool Infront(const glm::vec3& point) const;
	bool Behind(const glm::vec3& point) const;
	bool On(const glm::vec3& point) const;
};

void CreateSphere(ModelDef& md);

void CreateLine(ModelDef& md);
