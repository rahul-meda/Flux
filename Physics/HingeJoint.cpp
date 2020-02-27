
#include <glm/gtx/matrix_cross_product.hpp>
#include "HingeJoint.h"
#include "../Components/Body.h"
#include "../Graphics/Graphics.h"
#include "../Utils.h"

void HingeJointDef::Initialize(Body* bA, Body* bB, const glm::vec3& anchor, const glm::vec3& axis)
{
	bodyA = bA;
	bodyB = bB;
	Transform txA = bodyA->GetTransform();
	Transform txB = bodyB->GetTransform();

	localAnchorA = glm::transpose(txA.R) * (anchor - txA.position);
	localAnchorB = glm::transpose(txB.R) * (anchor - txB.position);
	localAxisA = glm::transpose(txA.R) * axis;
	localAxisB = glm::transpose(txB.R) * axis;
}

HingeJoint::HingeJoint(const HingeJointDef* def)
{
	bodyA = def->bodyA;
	bodyB = def->bodyB;
	localAnchorA = def->localAnchorA;
	localAnchorB = def->localAnchorB;
	localAxisA = def->localAxisA;
	localAxisB = def->localAxisB;
	impulseSumT = glm::vec3(0.0f);
	impulseSumR1 = 0.0f;
	impulseSumR2 = 0.0f;
}

void HingeJoint::InitVelocityConstraints(const SolverDef& def)
{
	indexA = bodyA->index;
	indexB = bodyB->index;
	localCenterA = bodyA->comL;
	localCenterB = bodyB->comL;
	mA = bodyA->invMass;
	mB = bodyB->invMass;
	iA = bodyA->iitW;
	iB = bodyB->iitW;
	std::vector<Position>* positions = def.positions;
	std::vector<Velocity>* velocities = def.velocities;

	glm::quat qA = (*positions)[indexA].q;
	glm::vec3 vA = (*velocities)[indexA].v;
	glm::vec3 wA = (*velocities)[indexA].w;
	glm::quat qB = (*positions)[indexB].q;
	glm::vec3 vB = (*velocities)[indexB].v;
	glm::vec3 wB = (*velocities)[indexB].w;

	glm::mat3 RA = glm::toMat3(qA);
	glm::mat3 RB = glm::toMat3(qB);

	// linear
	rA = RA * (localAnchorA - localCenterA);
	rB = RB * (localAnchorB - localCenterB);

	glm::mat3 rAx = glm::matrixCross3(rA);	// todo: check order - row vs column major
	glm::mat3 rBx = glm::matrixCross3(rB);
	glm::mat3 I = glm::identity<glm::mat3>();
	
	kT = mA * I - rAx * iA * rAx + mB * I - rBx * iB * rBx;	// todo: optimize

	// angular
	glm::vec3 aA = RA * localAxisA;	// world axis
	glm::vec3 aB = RB * localAxisB;
	ComputeBasis(aB, &t1, &t2);

	glm::vec3 correction = glm::cross(aA, aB);
	float erp = 0.7f * 60.0f;
	bias1 = erp * glm::dot(correction, t1);
	bias2 = erp * glm::dot(correction, t2);

	kr1 = glm::dot(iA * t1, t1) + glm::dot(iB * t1, t1);
	kr2 = glm::dot(iA * t2, t2) + glm::dot(iB * t2, t2);

	vA -= mA * impulseSumT;
	wA -= iA * glm::cross(rA, impulseSumT);

	vB += mB * impulseSumT;
	wB += iB * glm::cross(rB, impulseSumT);

	wA -= iA * t1 * impulseSumR1;
	wB += iB * t1 * impulseSumR1;

	wA -= iA * t2 * impulseSumR2;
	wB += iB * t2 * impulseSumR2;

	(*velocities)[indexA].v = vA;
	(*velocities)[indexA].w = wA;
	(*velocities)[indexB].v = vB;
	(*velocities)[indexB].w = wB;
}

void HingeJoint::SolveVelocityConstraints(const SolverDef& def)
{
	std::vector<Velocity>* velocities = def.velocities;
	glm::vec3 vA = (*velocities)[indexA].v;
	glm::vec3 wA = (*velocities)[indexA].w;
	glm::vec3 vB = (*velocities)[indexB].v;
	glm::vec3 wB = (*velocities)[indexB].w;

	glm::vec3 Cdot = vB + glm::cross(wB, rB) - vA - glm::cross(wA, rA);
	glm::vec3 impulseL(0.0f);
	if(glm::determinant(kT) != 0.0f)
		impulseL = glm::inverse(kT) * (-Cdot);	// todo: optimize

	impulseSumT += impulseL;

	vA -= mA * impulseL;
	wA -= iA * glm::cross(rA, impulseL);

	vB += mB * impulseL;
	wB += iB * glm::cross(rB, impulseL);

	float lambda1 = -(glm::dot(t1, wB - wA) + bias1) / kr1;
	impulseSumR1 += lambda1;

	wA -= iA * t1 * lambda1;
	wB += iB * t1 * lambda1;

	float lambda2 = -(glm::dot(t2, wB - wA) + bias2) / kr2;
	impulseSumR2 += lambda2;

	wA -= iA * t2 * lambda2;
	wB += iB * t2 * lambda2;

	(*velocities)[indexA].v = vA;
	(*velocities)[indexA].w = wA;
	(*velocities)[indexB].v = vB;
	(*velocities)[indexB].w = wB;
}

void HingeJoint::SolvePositionConstraints(const SolverDef& def)
{
	std::vector<Position>* positions = def.positions;

	glm::vec3 cA = (*positions)[indexA].c;
	glm::quat qA = (*positions)[indexA].q;
	glm::vec3 cB = (*positions)[indexB].c;
	glm::quat qB = (*positions)[indexB].q;

	rA = glm::toMat3(qA) * (localAnchorA - localCenterA);
	rB = glm::toMat3(qB) * (localAnchorB - localCenterB);

	glm::mat3 rAx = glm::matrixCross3(rA);
	glm::mat3 rBx = glm::matrixCross3(rB);
	glm::mat3 I = glm::identity<glm::mat3>();

	glm::mat3 K = mA * I - rAx * iA * rAx + mB * I - rBx * iB * rBx;

	glm::vec3 C = cB + rB - cA - rA;
	glm::vec3 P = -glm::inverse(K) * (C);

	float error = glm::length(C);

	cA -= mA * P;
	glm::vec3 w = iA * glm::cross(rA, P);
	qA -= 0.5f * glm::quat(0.0f, w) * qA;

	cB += mB * P;
	w = iB * glm::cross(rB, P);
	qB += 0.5f * glm::quat(0.0f, w) * qB;

	(*positions)[indexA].c = cA;
	(*positions)[indexA].q = qA;
	(*positions)[indexB].c = cB;
	(*positions)[indexB].q = qB;
}

void HingeJoint::Render()
{
	Transform txA = bodyA->tx;
	Transform txB = bodyB->tx;
	glm::vec3 anchor = txA.R * localAnchorA + txA.position;

	glm::vec3 cA = txA.R * localCenterA + txA.position;
	glm::vec3 cB = txB.R * localCenterB + txB.position;;
	glm::vec3 rA = anchor - cA;
	glm::vec3 rB = anchor - cB;
	glm::vec3 X(1.0f, 0.0f, 0.0f);
	glm::vec3 h = rA + X;
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
			lA = glm::length(rA);
			angle = acosf(rA.x / lA);
			axis = glm::normalize(axis);
		}
		q = glm::angleAxis(angle, axis);
	}
	Graphics::GetInstance().lines.push_back(R_Line(cA, q, glm::vec3(1.0f), 2.0f*lA));

	h = rB + X;
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
			lB = glm::length(rB);
			angle = acosf(rB.x / lB);
			axis = glm::normalize(axis);
		}
		q = glm::angleAxis(angle, axis);
	}
	Graphics::GetInstance().lines.push_back(R_Line(cB, q, glm::vec3(1.0f), 2.0f*lB));
}