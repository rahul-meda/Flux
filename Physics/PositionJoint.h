
#pragma once

#include <glm/glm.hpp>
#include "JointCommon.h"

// no relative translation b/w bodies wrt anchor point

class Body;

struct PositionJointDef
{
	PositionJointDef()
	{
		localAnchorA = glm::vec3(0.0f);
		localAnchorB = glm::vec3(0.0f);
	}

	void Initialize(Body* bodyA, Body* bodyB, const glm::vec3& anchor);

	Body* bodyA;
	Body* bodyB;
	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
};

class PositionJoint
{
public:
	PositionJoint(const PositionJointDef* def);

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
	glm::vec3 impulseSum;

	int indexA;
	int indexB;
	glm::vec3 rA;
	glm::vec3 rB;
	glm::vec3 localCenterA;
	glm::vec3 localCenterB;
	float mA;
	float mB;
	glm::mat3 iA;
	glm::mat3 iB;
	glm::mat3 effMass;			

	friend class Physics;
};

inline Body* PositionJoint::GetBodyA()
{
	return bodyA;
}

inline Body* PositionJoint::GetBodyB()
{
	return bodyB;
}