#pragma once

#include <glm/glm.hpp>

class Body;

struct SpringDef
{
	SpringDef()
	{
		l0 = 0.0f;
		w = 0.0f;
		localPA = glm::vec3(0.0f);
		localPB = glm::vec3(0.0f);
	}

	void Init(Body* A, Body* B, const glm::vec3& localPA, const glm::vec3& localPB);

	float l0;
	float w;
	float c;

	Body* A;
	Body* B;

	glm::vec3 localPA;
	glm::vec3 localPB;
};

class Spring
{
public:
	void Init(SpringDef* def);

	void Solve();

	void Render();

	float l0;
	float w;
	float k;
	float c;

	Body* A;
	Body* B;

	glm::vec3 localPA;
	glm::vec3 localPB;
};