#pragma once

#include <glm/glm.hpp>
#include "JointCommon.h"

class Body;

struct WheelJointDef
{
	WheelJointDef()
	{
		localAnchorA = glm::vec3(0.0f);
		localAnchorB = glm::vec3(0.0f);
		enableMotor = false;
		maxMotorTorque = 0.0f;
		motorSpeed = 0.0f;
		frequencyHz = 2.0f;
		dampingRatio = 0.7f;
	}

	void Initialize(Body* bodyA, Body* bodyB, const glm::vec3& anchor, const glm::vec3& axis);

	Body* bodyA;
	Body* bodyB;
	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
	glm::vec3 localAxisA;
	glm::vec3 localAxisB;
	bool enableMotor;
	float maxMotorTorque;
	float motorSpeed;
	float frequencyHz;
	float dampingRatio;
};

// 2 DOF joint. Simulates suspension b/w vehicle body, and wheel body
// Relative rotation allowed about joint axis
// Relative translation allowed along suspension axis (world Y by default)
// Spring-damper is used to smooth motion along suspension axis
class WheelJoint
{
public:
	glm::vec3 GetAnchorA() const;
	glm::vec3 GetAnchorB() const;

	// The local anchor point relative to bodyA's origin.
	const glm::vec3& GetLocalAnchorA() const { return localAnchorA; }

	// The local anchor point relative to bodyB's origin.
	const glm::vec3& GetLocalAnchorB() const { return localAnchorB; }

	// The local joint axis relative to bodyA.
	const glm::vec3& GetLocalAxisA() const { return localAxisA; }

	// Enable/disable the joint motor.
	void EnableMotor(bool flag);

	// Set the motor speed, usually in radians per second.
	void SetMotorSpeed(float speed);

	// Get the motor speed, usually in radians per second.
	float GetMotorSpeed() const;

	// Set/Get the maximum motor force, usually in N-m.
	void SetMaxMotorTorque(float torque);
	float GetMaxMotorTorque() const;

	// Get the current motor torque, usually in N-m.
	float GetMotorTorque() const;

	// Set/Get the spring frequency in hertz. Setting the frequency to zero disables the spring.
	void SetSpringFrequencyHz(float hz);
	float GetSpringFrequencyHz() const;

	// Set/Get the spring damping ratio
	void SetSpringDampingRatio(float ratio);
	float GetSpringDampingRatio() const;

	WheelJoint(const WheelJointDef* def);

	void InitVelocityConstraints(const SolverDef& def);
	void SolveVelocityConstraints(const SolverDef& def);
	void SolvePositionConstraints(const SolverDef& def);

private:

	Body* bodyA;
	Body* bodyB;

	// Solver shared
	glm::vec3 localAnchorA;
	glm::vec3 localAnchorB;
	glm::vec3 localAxisA;
	glm::vec3 localAxisB;

	float impulseSumT1, impulseSumT2, impulseSumR1, impulseSumR2;
	float motorImpulse;

	float maxMotorTorque;
	float motorSpeed;
	bool enableMotor;

	// Solver temp
	int indexA;
	int indexB;
	glm::vec3 localCenterA;
	glm::vec3 localCenterB;
	float invMassA;
	float invMassB;
	glm::mat3 invIA;
	glm::mat3 invIB;

	glm::vec3 ax, az;
	glm::vec3 sAx, sBx;
	glm::vec3 sAz, sBz;

	float kT1, kT2;
	float kM;

	glm::vec3 cr1;
	glm::vec3 cr2;
	float kr1;
	float kr2;
	float bias1, bias2;

	// spring
	float kS;
	float bS;
	float impulseSumS;
	float gamma;
	float frequencyHz;
	float dampingRatio;
	glm::vec3 ay;
	glm::vec3 sAy, sBy;
};

inline void WheelJoint::SetMotorSpeed(float speed)
{
	motorSpeed = speed;
}

inline float WheelJoint::GetMotorSpeed() const
{
	return motorSpeed;
}

inline float WheelJoint::GetMaxMotorTorque() const
{
	return maxMotorTorque;
}

inline void WheelJoint::SetSpringFrequencyHz(float hz)
{
	frequencyHz = hz;
}

inline float WheelJoint::GetSpringFrequencyHz() const
{
	return frequencyHz;
}

inline void WheelJoint::SetSpringDampingRatio(float ratio)
{
	dampingRatio = ratio;
}

inline float WheelJoint::GetSpringDampingRatio() const
{
	return dampingRatio;
}