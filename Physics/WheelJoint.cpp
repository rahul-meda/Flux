
#include "../Components/Body.h"
#include "WheelJoint.h"
#include "../Mesh/Geometry.h"

void WheelJointDef::Initialize(Body* bA, Body* bB, const glm::vec3& anchor, const glm::vec3& axis)
{
	bodyA = bA;
	bodyB = bB;
	Transform txA = bA->GetTransform();
	Transform txB = bB->GetTransform();

	localAnchorA = glm::transpose(txA.R) * (anchor - txA.position);
	localAnchorB = glm::transpose(txB.R) * (anchor - txB.position);
	localAxisA = glm::transpose(txA.R) * axis;
	localAxisB = glm::transpose(txB.R) * axis;
}

WheelJoint::WheelJoint(const WheelJointDef* def)
{
	bodyA = def->bodyA;
	bodyB = def->bodyB;
	localAnchorA = def->localAnchorA;
	localAnchorB = def->localAnchorB;
	localAxisA = def->localAxisA;
	localAxisB = def->localAxisB;

	kT1 = kT2 = 0.0f;
	impulseSumT1 = impulseSumT2 = impulseSumR1 = impulseSumR2 = 0.0f;
	kM = 0.0f;
	motorImpulse = 0.0f;
	impulseSumS = 0.0f;

	maxMotorTorque = def->maxMotorTorque;
	motorSpeed = def->motorSpeed;
	enableMotor = def->enableMotor;

	frequencyHz = def->frequencyHz;
	dampingRatio = def->dampingRatio;

	ax = glm::vec3(0.0f);
	az = glm::vec3(0.0f);
}

void WheelJoint::InitVelocityConstraints(const SolverDef& def)
{
	indexA = bodyA->index;
	indexB = bodyB->index;
	localCenterA = bodyA->comL;
	localCenterB = bodyB->comL;
	invMassA = bodyA->invMass;
	invMassB = bodyB->invMass;
	invIA = bodyA->iitW;
	invIB = bodyB->iitW;

	float mA = invMassA, mB = invMassB;
	glm::mat3 iA = invIA, iB = invIB;

	std::vector<Position>* positions = def.positions;
	std::vector<Velocity>* velocities = def.velocities;

	glm::vec3 cA = (*positions)[indexA].c;
	glm::quat qA = (*positions)[indexA].q;
	glm::vec3 vA = (*velocities)[indexA].v;
	glm::vec3 wA = (*velocities)[indexA].w;
	glm::vec3 cB = (*positions)[indexB].c;
	glm::quat qB = (*positions)[indexB].q;
	glm::vec3 vB = (*velocities)[indexB].v;
	glm::vec3 wB = (*velocities)[indexB].w;

	glm::mat3 RA = glm::toMat3(qA);
	glm::mat3 RB = glm::toMat3(qB);

	// Compute the effective masses.
	glm::vec3 rA = RA * (localAnchorA - localCenterA);
	glm::vec3 rB = RB * (localAnchorB - localCenterB);

	// Point to point constraint
	{
		ax = RA * glm::vec3(1.0f, 0.0f, 0.0f);
		sAx = glm::cross(rA, ax);
		sBx = glm::cross(rB, ax);

		kT1 = mA + mB + glm::dot(iA * sAx, sAx) + glm::dot(iB * sBx, sBx);

		if (kT1 > 0.0f)
		{
			kT1 = 1.0f / kT1;
		}

		az = RA * localAxisA;
		sAz = glm::cross(rA, az);
		sBz = glm::cross(rB, az);

		kT2 = mA + mB + glm::dot(iA * sAz, sAz) + glm::dot(iB * sBz, sBz);

		if (kT2 > 0.0f)
		{
			kT2 = 1.0f / kT2;
		}
	}

	// Spring constraint
	kS = 0.0f;
	bS = 0.0f;
	gamma = 0.0f;
	glm::vec3 d = cB + rB - cA - rA;
	if (frequencyHz > 0.0f)
	{
		ay = RA * glm::vec3(0.0f, 1.0f, 0.0f);
		sAy = glm::cross(rA, ay);
		sBy = glm::cross(rB, ay);

		float invMass = mA + mB + glm::dot(iA * sAy, sAy) + glm::dot(iB * sBy, sBy);

		if (invMass > 0.0f)
		{
			kS = 1.0f / invMass;

			float C = glm::dot(d, ay);

			// Frequency
			float omega = 2.0f * PI * frequencyHz;

			// Damping coefficient
			float damp = 2.0f * kS * dampingRatio * omega;

			// Spring stiffness
			float k = kS * omega * omega;

			// magic formulas
			float h = 1.0f / hertz;
			gamma = h * (damp + h * k);
			if (gamma > 0.0f)
			{
				gamma = 1.0f / gamma;
			}

			bS = C * h * k * gamma;

			kS = invMass + gamma;
			if (kS > 0.0f)
			{
				kS = 1.0f / kS;
			}
		}
	}
	else
	{
		impulseSumS = 0.0f;
	}

	{
		// angular
		glm::vec3 t1, t2;
		ComputeBasis(localAxisB, &t1, &t2);
		glm::vec3 d = RB * t1;
		glm::vec3 e = RB * t2;

		cr1 = glm::cross(d, az);
		kr1 = glm::dot((iA * cr1), cr1) + glm::dot((iB * cr1), cr1);

		cr2 = glm::cross(e, az);
		kr2 = glm::dot((iA * cr2), cr2) + glm::dot((iB * cr2), cr2);

		bias1 = 2.0f * baumgarte * hertz * glm::dot(az, d);
		bias2 = 2.0f * baumgarte * hertz * glm::dot(az, e);

		// Rotational motor
		if (enableMotor)
		{
			kM = glm::dot(iA * az, az) + glm::dot(iB * az, az);

			if (kM > 0.0f)
			{
				kM = 1.0f / kM;
			}
		}
		else
		{
			kM = 0.0f;
			motorImpulse = 0.0f;
		}
	}

	// warmsrating
	glm::vec3 P = impulseSumT1 * ax + impulseSumT2 * az + impulseSumS * ay;
	glm::vec3 LA = impulseSumT1 * sAx + impulseSumT2 * sAz + impulseSumS * sAy;
	glm::vec3 LB = impulseSumT1 * sBx + impulseSumT2 * sBz + impulseSumS * sBy;

	vA -= mA * P;
	wA -= iA * LA;

	vB += mB * P;
	wB += iB * LB;

	// warmstart angular and motor
}

void WheelJoint::SolveVelocityConstraints(const SolverDef& def)
{
	float mA = invMassA, mB = invMassB;
	glm::mat3 iA = invIA, iB = invIB;
	std::vector<Velocity>* velocities = def.velocities;

	glm::vec3 vA = (*velocities)[indexA].v;
	glm::vec3 wA = (*velocities)[indexA].w;
	glm::vec3 vB = (*velocities)[indexB].v;
	glm::vec3 wB = (*velocities)[indexB].w;

	// Solve spring constraint
	{
		float Cdot = glm::dot(ay, vB - vA) + glm::dot(sBy, wB) - glm::dot(sAy, wA);
		float impulse = -kS * (Cdot + bS + gamma * impulseSumS);
		impulseSumS += impulse;

		glm::vec3 P = impulse * ay;
		glm::vec3 LA = impulse * sAy;
		glm::vec3 LB = impulse * sBy;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	// Solve rotational motor constraint
	if (enableMotor)
	{
		float cDot = glm::dot(wB - wA, az) + motorSpeed;
		float impulse = -cDot / kM;
		float oldImpulse = motorImpulse;
		motorImpulse = glm::clamp(motorImpulse + impulse, -maxMotorTorque, maxMotorTorque);
		impulse = motorImpulse - oldImpulse;

		wA -= iA * az * impulse;
		wB += iB * az * impulse;
	}

	// Solve point to line constraint
	{
		float Cdot = glm::dot(ax, vB - vA) + glm::dot(sBx, wB) - glm::dot(sAx, wA);
		float impulse = -kT1 * Cdot;
		impulseSumT1 += impulse;

		glm::vec3 P = impulse * ax;
		glm::vec3 LA = impulse * sAx;
		glm::vec3 LB = impulse * sBx;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;

		Cdot = glm::dot(az, vB - vA) + glm::dot(sBz, wB) - glm::dot(sAz, wA);
		impulse = -kT2 * Cdot;
		impulseSumT2 += impulse;

		P = impulse * az;
		LA = impulse * sAz;
		LB = impulse * sBz;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	// angular
	float CdotR1 = glm::dot(cr1, wB - wA);
	float l1 = -(CdotR1 + bias1) / kr1;
	impulseSumR1 += l1;

	wA -= iA * cr1 * l1;
	wB += iB * cr1 * l1;

	float CdotR2 = glm::dot(cr2, wB - wA);
	float l2 = -(CdotR2 + bias2) / kr2;
	impulseSumR2 += l2;

	wA -= iA * cr2 * l2;
	wB += iB * cr2 * l2;

	(*velocities)[indexA].v = vA;
	(*velocities)[indexA].w = wA;
	(*velocities)[indexB].v = vB;
	(*velocities)[indexB].w = wB;
}

void WheelJoint::SolvePositionConstraints(const SolverDef& def)
{
	std::vector<Position>* positions = def.positions;

	glm::vec3 cA = (*positions)[indexA].c;
	glm::quat qA = (*positions)[indexA].q;
	glm::vec3 cB = (*positions)[indexB].c;
	glm::quat qB = (*positions)[indexB].q;

	glm::mat3 RA = glm::toMat3(qA);
	glm::mat3 RB = glm::toMat3(qB);

	glm::vec3 rA = RA * (localAnchorA - localCenterA);
	glm::vec3 rB = RB * (localAnchorB - localCenterB);
	glm::vec3 d = (cB - cA) + rB - rA;

	glm::vec3 ax = RA * glm::vec3(1.0f, 0.0f, 0.0f);

	glm::vec3 sAx = glm::cross(rA, ax);
	glm::vec3 sBx = glm::cross(rB, ax);

	float C = glm::dot(d, ax);

	float k = invMassA + invMassB + glm::dot(invIA * sAx, sAx) + glm::dot(invIB * sBx, sBx);

	float impulse;
	if (k != 0.0f)
	{
		impulse = -C / k;
	}
	else
	{
		impulse = 0.0f;
	}

	glm::vec3 P = impulse * ax;
	glm::vec3 LA = impulse * sAx;
	glm::vec3 LB = impulse * sBx;

	cA -= invMassA * P;
	glm::vec3 w = invIA * LA;
	qA -= 0.5f * glm::quat(0.0f, w) * qA;
	cB += invMassB * P;
	w = invIB * LB;
	qB += 0.5f * glm::quat(0.0f, w) * qB;

	glm::vec3 az = RA * localAxisA;

	glm::vec3 sAz = glm::cross(rA, az);
	glm::vec3 sBz = glm::cross(rB, az);

	C = glm::dot(d, az);

	k = invMassA + invMassB + glm::dot(invIA * sAz, sAz) + glm::dot(invIB * sBz, sBz);

	if (k != 0.0f)
	{
		impulse = -C / k;
	}
	else
	{
		impulse = 0.0f;
	}

	P = impulse * az;
	LA = impulse * sAz;
	LB = impulse * sBz;

	cA -= invMassA * P;
	w = invIA * LA;
	qA -= 0.5f * glm::quat(0.0f, w) * qA;
	cB += invMassB * P;
	w = invIB * LB;
	qB += 0.5f * glm::quat(0.0f, w) * qB;

	(*positions)[indexA].c = cA;
	(*positions)[indexA].q = qA;
	(*positions)[indexB].c = cB;
	(*positions)[indexB].q = qB;
}

