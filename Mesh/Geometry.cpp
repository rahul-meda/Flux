
#include "Geometry.h"

#define EPSILON 0.005f

HalfSpace::HalfSpace()
{}

HalfSpace::HalfSpace(const glm::vec3& normal, float distance)
	: normal(normal), distance(distance)
{}

HalfSpace::HalfSpace(const glm::vec3& normal, const glm::vec3& point)
	: normal(normal), distance(glm::dot(point, normal))
{}

glm::vec3 HalfSpace::Origin() const
{
	return distance * normal;
}

float HalfSpace::Distance(const glm::vec3& point) const
{
	return glm::dot(point, normal) - distance;
}

glm::vec3 HalfSpace::Projection(const glm::vec3& point) const
{
	return point - Distance(point) * normal;
}

bool HalfSpace::Infront(const glm::vec3& point) const
{
	return Distance(point) > 0.0f;
}

bool HalfSpace::Behind(const glm::vec3& point) const
{
	return Distance(point) < 0.0f;
}

bool HalfSpace::On(const glm::vec3& point) const
{
	float dist = Distance(point);

	return (dist < EPSILON && dist > -EPSILON);
}

void CreateSphere(ModelDef& md)
{
	const int slices = 20;
	const int stacks = 20;
	float R = 1.0f / (float)(slices - 1);
	float S = 1.0f / (float)(stacks - 1);
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;

	for (int r = 0; r < slices; ++r)
	{
		for (int s = 0; s < stacks; ++s)
		{
			float y = (float)(sin(-PI*0.5f + PI * r * R));
			float x = (float)(cos(2 * PI * s * S) * sin(PI * r * R));
			float z = (float)(sin(2 * PI * s * S) * sin(PI * r * R));

			vertices.push_back(glm::vec3(x, y, z));

			int curRow = r * stacks;
			int nextRow = (r + 1) * stacks;

			indices.push_back(curRow + s);
			indices.push_back(nextRow + s);
			indices.push_back(nextRow + (s + 1));

			indices.push_back(curRow + s);
			indices.push_back(nextRow + (s + 1));
			indices.push_back(curRow + (s + 1));
		}
	}

	int N = indices.size();
	for (int i = 0; i < N; ++i)
	{
		if (indices[i] > slices * stacks - 1) continue;
		md.vertices.push_back(vertices[indices[i]]);
		md.normals.push_back(vertices[indices[i]]);
	}
}

void CreateLine(ModelDef& md)
{
	md.vertices.push_back(glm::vec3(0.0f));
	md.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	md.vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
	md.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
}