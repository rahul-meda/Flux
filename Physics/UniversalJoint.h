
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "JointCommon.h"

// no relative translation b/w bodies wrt anchor point

class Body;

struct UniversalJointDef
{
	UniversalJointDef()
	{
		localAnchorA = glm::vec3(0.0f);
		localAnchorB = glm::vec3(0.0f);
		enableLimit = false;
		lowerLimit = 0.0f;
		upperLimit = 0.0f;
		enableMotor = false;
		motorSpeed = 0.0f;
		maxMotorTorque = 0.2f;
	}

	void Initialize(Body* bodyA, Body* bodyB, const glm::vec3& anchor, const glm::vec3& axisA, const glm::vec3& axisB);

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
	bool enableMotor;
	float motorSpeed;
	float maxMotorTorque;
	float scale;	// render
};

class UniversalJoint
{
public:
	UniversalJoint(const UniversalJointDef* def);

	void InitVelocityConstraints(const SolverDef& def);
	void SolveVelocityConstraints(const SolverDef& def);
	void SolvePositionConstraints(const SolverDef& def);

	void Render();

	Body* GetBodyA();
	Body* GetBodyB();
	void SetMotorSpeed(float speed);
	float GetMotorSpeed();

private:
	Body* bodyA;
	Body* bodyB;

	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
	glm::vec3 localAxisA;
	glm::vec3 localAxisB;
	glm::vec3 f1;	// world hinge axis
	glm::vec3 f2;
	glm::vec3 f;
	glm::quat q0;	// initial relative orientation
	glm::vec3 impulseSumT;
	float impulseSumR;
	float impulseSumR1, impulseSumR2;

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
	float kr;
	float bias1, bias2;

	//angular limit
	bool enableLimit;
	float lowerLimit, upperLimit;
	float kMin, kMax;
	float bMin, bMax;		// limits correction bias
	float jSign;				// to ensure correct Jacobian direction for correcting limits
	LimitState limitState;

	bool enableMotor;
	float motorSpeed;
	float maxMotorTorque;
	float kMotor;
	float impulseMotor;

	float scale;	// render

	friend class Physics;
};

inline Body* UniversalJoint::GetBodyA()
{
	return bodyA;
}

inline Body* UniversalJoint::GetBodyB()
{
	return bodyB;
}

inline void UniversalJoint::SetMotorSpeed(float speed)
{
	motorSpeed = speed;
}

inline float UniversalJoint::GetMotorSpeed()
{
	return motorSpeed;
}