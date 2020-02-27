
#pragma once

#include <glm/glm.hpp>
#include "JointCommon.h"

// no relative translation b/w bodies wrt anchor point

class Body;

struct HingeJointDef
{
	HingeJointDef()
	{
		localAnchorA = glm::vec3(0.0f);
		localAnchorB = glm::vec3(0.0f);
	}

	void Initialize(Body* bodyA, Body* bodyB, const glm::vec3& anchor, const glm::vec3& axis);

	Body* bodyA;
	Body* bodyB;
	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
	glm::vec3 localAxisA;
	glm::vec3 localAxisB;
};

class HingeJoint
{
public:
	HingeJoint(const HingeJointDef* def);

	void InitVelocityConstraints(const SolverDef& def);
	void SolveVelocityConstraints(const SolverDef& def);
	void SolvePositionConstraints(const SolverDef& def);

	void Render();

	Body* GetBodyA();
	Body* GetBodyB();

private:
	Body* bodyA;
	Body* bodyB;

	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
	glm::vec3 localAxisA;
	glm::vec3 localAxisB;
	glm::vec3 impulseSumT;
	float impulseSumR1;
	float impulseSumR2;

	int indexA;
	int indexB;
	glm::vec3 rA;	// world space anchor
	glm::vec3 rB;
	glm::vec3 t1, t2;	// orthogonal to world axis
	glm::vec3 localCenterA;
	glm::vec3 localCenterB;
	float mA;
	float mB;
	glm::mat3 iA;
	glm::mat3 iB;
	glm::mat3 kT;
	float kr1;
	float kr2;
	float bias1, bias2;

	friend class Physics;
};

inline Body* HingeJoint::GetBodyA()
{
	return bodyA;
}

inline Body* HingeJoint::GetBodyB()
{
	return bodyB;
}
