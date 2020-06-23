
#include "SpringConstraint.h"
#include "../Components/Body.h"
#include "../Graphics/Graphics.h"
#include "Settings.h"

void SpringDef::Init(Body* bA, Body* bB, const glm::vec3& PA, const glm::vec3& PB)
{
	A = bA;
	B = bB;
	Transform txA = A->GetTransform();
	Transform txB = B->GetTransform();
	localPA = glm::transpose(txA.R) * (PA - txA.position);
	localPB = glm::transpose(txB.R) * (PB - txB.position);
}

void Spring::Init(SpringDef* def)
{
	A = def->A;
	B = def->B;
	localPA = def->localPA;
	localPB = def->localPB;
	l0 = def->l0;
	w = def->w;
	k = 2000.0f;
	c = def->c;
}

void Spring::Solve()
{
	Transform txA = A->GetTransform();
	Transform txB = B->GetTransform();

	glm::vec3 PA = txA.R * localPA + txA.position;
	glm::vec3 PB = txB.R * localPB + txB.position;

	float l = glm::length(PB - PA);
	assert(l != 0.0f);
	float x = l - l0;

	glm::vec3 dir = (PB - PA) / l;
	glm::vec3 rA = PA - A->GetCentroid();
	glm::vec3 rB = PB - B->GetCentroid();
	glm::vec3 vA = A->GetVelocity();
	glm::vec3 wA = A->GetAngularVelocity();
	glm::vec3 vB = B->GetVelocity();
	glm::vec3 wB = B->GetAngularVelocity();
	glm::vec3 vRel = vB + glm::cross(wB, rB) - vA - glm::cross(wA, rA);
	float v = glm::dot(vRel, dir);

	glm::vec3 F = (k * x + c * v) * dir;

	A->ApplyForce(F, PA);
	B->ApplyForce(-F, PB);

	glm::vec3 X(1.0f, 0.0f, 0.0f);
	glm::vec3 h = dir + X;
	glm::quat q = glm::identity<glm::quat>();
	float angle = 0.0f;
	float lA = 0.0f, lB = 0.0f;
	if (glm::length2(h) == 0.0f)
	{
		q = glm::angleAxis(PI, glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else
	{
		h = glm::normalize(h);
		glm::vec3 axis(0.0f, -h.z, h.y);
		if (glm::length2(axis) == 0.0f)
			axis = glm::vec3(0.0f, 1.0f, 0.0f);
		else
		{
			angle = acosf(dir.x);
			axis = glm::normalize(axis);
		}
		q = glm::angleAxis(angle, axis);
	}
	Graphics::GetInstance().lines.push_back(R_Line(PA, q, glm::vec3(1.0f), 2.0f * l));
}

void Spring::Render()
{

}