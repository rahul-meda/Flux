
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "JointCommon.h"

// no relative translation b/w bodies wrt anchor point

class Body;

struct HingeJointDef
{
	HingeJointDef()
	{
		localAnchorA = glm::vec3(0.0f);
		localAnchorB = glm::vec3(0.0f);
		enableLimit = false;
		lowerLimit = 0.0f;
		upperLimit = 0.0f;
	}

	void Initialize(Body* bodyA, Body* bodyB, const glm::vec3& anchor, const glm::vec3& axis);

	Body* bodyA;
	Body* bodyB;
	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
	glm::vec3 localAxisA;
	glm::vec3 localAxisB;
	glm::quat q0;
	bool enableLimit;
	float lowerLimit;	// [0 2*PI] todo: verify
	float upperLimit;	// [0 2*PI]

	float scale;	// render
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
	glm::vec3 f;	// world hinge axis
	glm::quat q0;	// initial relative orientation
	glm::vec3 impulseSumT;
	float impulseSumR;

	int indexA;
	int indexB;
	glm::vec3 rA;	// world space anchor
	glm::vec3 rB;
	glm::vec3 localCenterA;
	glm::vec3 localCenterB;
	float mA;
	float mB;
	glm::mat3 iA;
	glm::mat3 iB;
	glm::mat3 kT;
	glm::vec3 cr1;
	glm::vec3 cr2;
	float kr1;
	float kr2;
	float bias1, bias2;

	//angular limit
	bool enableLimit;
	float lowerLimit, upperLimit;
	float kMin, kMax;
	float bMin, bMax;		// limits correction bias
	bool flipJ;				// flip tje jacobian sigh based on relative quaternion axis
	LimitState limitState;

	float scale;	// render

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
