
#include "ContactSolver.h" 
#include "../Components/Body.h"
#include "Contact.h"
#include "Collider.h"
#include "../Utils.h"
#include <iostream>

struct ContactPositionConstraint
{
	glm::vec3 localPoints[maxManifoldPoints];
	glm::vec3 localNormal;
	glm::vec3 localPoint;
	int indexA, indexB;
	float invMassA, invMassB;
	glm::vec3 localCenterA, localCenterB;
	glm::mat3 invIA, invIB;
	Manifold::Type type;
	float radiusA, radiusB;
	int pointCount;
};

ContactSolver::ContactSolver(ContactSolverDef* def)
{
	contacts = def->contacts;
	positions = def->positions;
	velocities = def->velocities;
	int nContacts = (*contacts).size();
	positionConstraints.reserve(nContacts);
	velocityConstraints.reserve(nContacts);

	// Initialize position independent portions of the constraints.
	for (int i = 0; i < nContacts; ++i)
	{
		Contact* contact = (*contacts)[i];

		if (contact->touching == false)
			continue;

		Collider* colliderA = contact->GetColliderA();
		Collider* colliderB = contact->GetColliderB();
		Body* bodyA = colliderA->GetBody();
		Body* bodyB = colliderB->GetBody();
		Manifold* manifold = contact->GetManifold();

		int pointCount = manifold->nPoints;
		assert(pointCount > 0);

		ContactVelocityConstraint vc;
		vc.friction = contact->GetFriction();
		vc.restitution = contact->GetRestitution();
		vc.indexA = bodyA->index;
		vc.indexB = bodyB->index;
		vc.invMassA = bodyA->invMass;
		vc.invMassB = bodyB->invMass;
		vc.invIA = bodyA->iitW;
		vc.invIB = bodyB->iitW;
		vc.contactIndex = i;
		vc.pointCount = pointCount;
		vc.K = glm::mat3(0.0f);
		vc.normalMass = glm::mat3(0.0f);

		ContactPositionConstraint pc;
		pc.indexA = bodyA->index;
		pc.indexB = bodyB->index;
		pc.invMassA = bodyA->invMass;
		pc.invMassB = bodyB->invMass;
		pc.localCenterA = bodyA->comL;
		pc.localCenterB = bodyB->comL;
		pc.invIA = bodyA->iitW;
		pc.invIB = bodyB->iitW;
		pc.localNormal = manifold->localNormal;
		pc.localPoint = manifold->localPoint;
		pc.pointCount = pointCount;
		pc.radiusA = colliderA->radius;
		pc.radiusB = colliderB->radius;
		pc.type = manifold->type;

		for (int j = 0; j < pointCount; ++j)
		{
			ManifoldPoint* cp = manifold->points + j;
			VelocityConstraintPoint* vcp = vc.points + j;

			vcp->normalImpulse = cp->normalImpulse;
			vcp->tangentImpulse[0] = cp->tangentImpulse[0];
			vcp->tangentImpulse[1] = cp->tangentImpulse[1];
			vcp->rA = glm::vec3(0.0f);
			vcp->rB = glm::vec3(0.0f);
			vcp->normalMass = 0.0f;
			vcp->tangentMass[0] = 0.0f;
			vcp->tangentMass[1] = 0.0f;
			vcp->velocityBias = 0.0f;

			pc.localPoints[j] = cp->localPoint;
		}
		velocityConstraints.push_back(vc);
		positionConstraints.push_back(pc);
	}
}

ContactSolver::~ContactSolver()
{
	velocityConstraints.clear();
	positionConstraints.clear();
}

// Initialize position dependent portions of the velocity constraints.
void ContactSolver::InitializeVelocityConstraints()
{
	for (int i = 0; i < velocityConstraints.size(); ++i)
	{
		ContactVelocityConstraint* vc = &velocityConstraints[i];
		ContactPositionConstraint* pc = &positionConstraints[i];
		
		Manifold* manifold = (*contacts)[vc->contactIndex]->GetManifold();

		int indexA = vc->indexA;
		int indexB = vc->indexB;

		float mA = vc->invMassA;
		float mB = vc->invMassB;
		glm::mat3 iA = vc->invIA;
		glm::mat3 iB = vc->invIB;
		glm::vec3 localCenterA = pc->localCenterA;
		glm::vec3 localCenterB = pc->localCenterB;

		glm::vec3 cA = (*positions)[indexA].c;
		glm::quat qA = (*positions)[indexA].q;
		glm::vec3 vA = (*velocities)[indexA].v;
		glm::vec3 wA = (*velocities)[indexA].w;

		glm::vec3 cB = (*positions)[indexB].c;
		glm::quat qB = (*positions)[indexB].q;
		glm::vec3 vB = (*velocities)[indexB].v;
		glm::vec3 wB = (*velocities)[indexB].w;

		assert(manifold->nPoints > 0);

		Transform txA, txB;
		txA.R = glm::toMat3(qA);
		txB.R = glm::toMat3(qB);
		txA.position = cA - (txA.R * localCenterA);
		txB.position = cB - (txB.R * localCenterB);

		WorldManifold worldManifold;
		worldManifold.Initialize(manifold, txA, txB, pc->radiusA, pc->radiusB);

		vc->normal = worldManifold.normal;
		vc->tangent[0] = worldManifold.tangent[0];
		vc->tangent[1] = worldManifold.tangent[1];

		int pointCount = vc->pointCount;
		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			vcp->rA = worldManifold.points[j] - cA;
			vcp->rB = worldManifold.points[j] - cB;

			glm::vec3 rnA = glm::cross(vcp->rA, vc->normal);
			glm::vec3 rnB = glm::cross(vcp->rB, vc->normal);

			float kNormal = mA + mB + glm::dot(iA * rnA,  rnA) + glm::dot(iB * rnB,  rnB);

			vcp->normalMass = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

			for (int i = 0; i < 2; ++i)
			{
				glm::vec3 rtA = glm::cross(vcp->rA, vc->tangent[i]);
				glm::vec3 rtB = glm::cross(vcp->rB, vc->tangent[i]);

				float kTangent = mA + mB + glm::dot(iA * rtA, rtA) + glm::dot(iB * rtB, rtB);
				vcp->tangentMass[i] = kTangent > 0.0f ? 1.0f / kTangent : 0.0f;
			}

			// Setup a velocity bias for restitution.
			vcp->velocityBias = 0.0f;
			float vRel = glm::dot(vc->normal, vB + glm::cross(wB, vcp->rB) - vA - glm::cross(wA, vcp->rA));
			if (vRel < -velocityThreshold)
			{
				vcp->velocityBias = -vc->restitution * vRel;
			}
		}
	}
}

void ContactSolver::WarmStart()
{
	// Warm start.
	for (int i = 0; i < velocityConstraints.size(); ++i)
	{
		ContactVelocityConstraint* vc = &velocityConstraints[i];

		int indexA = vc->indexA;
		int indexB = vc->indexB;
		float mA = vc->invMassA;
		glm::mat3 iA = vc->invIA;
		float mB = vc->invMassB;
		glm::mat3 iB = vc->invIB;
		int pointCount = vc->pointCount;

		glm::vec3 vA = (*velocities)[indexA].v;
		glm::vec3 wA = (*velocities)[indexA].w;
		glm::vec3 vB = (*velocities)[indexB].v;
		glm::vec3 wB = (*velocities)[indexB].w;

		glm::vec3 normal = vc->normal;

		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;
			glm::vec3 P = vcp->normalImpulse * normal + vcp->tangentImpulse[0] * vc->tangent[0];
			P += vcp->tangentImpulse[1] * vc->tangent[1];
			wA -= iA * glm::cross(vcp->rA, P);
			vA -= mA * P;
			wB += iB * glm::cross(vcp->rB, P);
			vB += mB * P;
		}

		(*velocities)[indexA].v = vA;
		(*velocities)[indexA].w = wA;
		(*velocities)[indexB].v = vB;
		(*velocities)[indexB].w = wB;
	}
}

void ContactSolver::SolveVelocityConstraints()
{
	for (int i = 0; i < velocityConstraints.size(); ++i)
	{
		ContactVelocityConstraint* vc = &velocityConstraints[i];
		
		int indexA = vc->indexA;
		int indexB = vc->indexB;
		float mA = vc->invMassA;
		glm::mat3 iA = vc->invIA;
		float mB = vc->invMassB;
		glm::mat3 iB = vc->invIB;
		int pointCount = vc->pointCount;

		glm::vec3 vA = (*velocities)[indexA].v;
		glm::vec3 wA = (*velocities)[indexA].w;
		glm::vec3 vB = (*velocities)[indexB].v;
		glm::vec3 wB = (*velocities)[indexB].w;

		glm::vec3 normal = vc->normal;
		float friction = vc->friction;

		// Solve tangent constraints first because non-penetration is more important
		// than friction.
		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			// Relative velocity at contact
			glm::vec3 dv = vB + glm::cross(wB, vcp->rB) - vA - glm::cross(wA, vcp->rA);

			for (int i = 0; i < 2; ++i)
			{
				// Compute tangent force
				float vt = glm::dot(dv, vc->tangent[i]);
				float lambda[2];
				lambda[i] = vcp->tangentMass[i] * (-vt);

				// b2Clamp the accumulated force
				float maxFriction = friction * vcp->normalImpulse;
				float newImpulse = glm::clamp(vcp->tangentImpulse[i] + lambda[i], -maxFriction, maxFriction);
				lambda[i] = newImpulse - vcp->tangentImpulse[i];
				vcp->tangentImpulse[i] = newImpulse;

				// Apply contact impulse
				glm::vec3 P = lambda[i] * vc->tangent[i];

				vA -= mA * P;
				wA -= iA * glm::cross(vcp->rA, P);

				vB += mB * P;
				wB += iB * glm::cross(vcp->rB, P);
			}
		}

		// Solve normal constraints
		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			// Relative velocity at contact
			glm::vec3 dv = vB + glm::cross(wB, vcp->rB) - vA - glm::cross(wA, vcp->rA);

			// Compute normal impulse
			float vn = glm::dot(dv, normal);
			float lambda = -vcp->normalMass * (vn - vcp->velocityBias);

			// b2Clamp the accumulated impulse
			float newImpulse = glm::max(vcp->normalImpulse + lambda, -0.2f);	// magnetic force
			lambda = newImpulse - vcp->normalImpulse;
			vcp->normalImpulse = newImpulse;

			// Apply contact impulse
			glm::vec3 P = lambda * normal;
			vA -= mA * P;
			wA -= iA * glm::cross(vcp->rA, P);

			vB += mB * P;
			wB += iB * glm::cross(vcp->rB, P);
		}

		(*velocities)[indexA].v = vA;
		(*velocities)[indexA].w = wA;
		(*velocities)[indexB].v = vB;
		(*velocities)[indexB].w = wB;
	}
}

void ContactSolver::StoreImpulses()
{
	for (int i = 0; i < velocityConstraints.size(); ++i)
	{
		ContactVelocityConstraint* vc = &velocityConstraints[i];
		Manifold* manifold = (*contacts)[vc->contactIndex]->GetManifold();

		for (int j = 0; j < vc->pointCount; ++j)
		{
			manifold->points[j].normalImpulse = vc->points[j].normalImpulse;
			manifold->points[j].tangentImpulse[0] = vc->points[j].tangentImpulse[0];
			manifold->points[j].tangentImpulse[1] = vc->points[j].tangentImpulse[1];
		}
	}
}

struct b2PositionSolverManifold
{
	void Initialize(ContactPositionConstraint* pc, const Transform& txA, const Transform& txB, int index)
	{
		assert(pc->pointCount > 0);

		switch (pc->type)
		{
		case Manifold::sphere:
		{
			normal = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 pointA = txA.R * pc->localPoint + txA.position;
			glm::vec3 pointB = txB.R * pc->localPoints[0] + txB.position;
			if (glm::distance2(pointA, pointB) > fxEpsilon * fxEpsilon)
			{
				normal = pointB - pointA;
				normal = glm::normalize(normal);
			}
			glm::vec3 cA = pointA + pc->radiusA * normal;
			glm::vec3 cB = pointB - pc->radiusB * normal;
			point = 0.5f * (cA + cB);
			//point = 0.5f * (pointA + pointB);
			separation = glm::dot(pointB - pointA, normal);// -pc->radiusA - pc->radiusB;
		}
		break;

		case Manifold::faceA:
		{
			normal = txA.R * pc->localNormal;
			glm::vec3 planePoint = txA.R * pc->localPoint + txA.position;

			glm::vec3 clipPoint = txB.R * pc->localPoints[index] + txB.position;
			/*separation = glm::dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
			point = clipPoint;*/
			glm::vec3 cA = clipPoint + normal * (pc->radiusA - glm::dot(clipPoint - planePoint, normal));
			glm::vec3 cB = clipPoint - normal * pc->radiusB;
			point = (cA + cB) * 0.5f;
			separation = glm::dot(cB - cA, normal);
		}
		break;

		case Manifold::faceB:
		{
			normal = txB.R * pc->localNormal;
			glm::vec3 planePoint = txB.R * pc->localPoint + txB.position;

			glm::vec3 clipPoint = txA.R * pc->localPoints[index] + txA.position;
			/*separation = glm::dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
			point = clipPoint;*/

			glm::vec3 cB = clipPoint + normal * (pc->radiusB - glm::dot(clipPoint - planePoint, normal));
			glm::vec3 cA = clipPoint - normal * pc->radiusA;
			point = (cA + cB) * 0.5f;
			separation = glm::dot(cA - cB, normal);

			// Ensure normal points from A to B
			normal = -normal;
		}
		break;

		case Manifold::edges:
		{
			glm::vec3 PA = txA.R * pc->localPoint + txA.position;
			glm::vec3 PB = txB.R * pc->localPoints[index] + txB.position;
			normal = txB.R * pc->localNormal;
			glm::vec3 CA = PA - pc->radiusA * normal; // TODO: check sign
			glm::vec3 CB = PB + pc->radiusB * normal;
			point = (CA + CB) * 0.5f;
			separation = glm::dot(CB - CA, normal);
		}
		break;
		}
	}

	glm::vec3 normal;
	glm::vec3 point;
	float separation;
};

bool ContactSolver::SolvePositionConstraints()
{
	float minSeparation = 0.0f;

	for (int i = 0; i < positionConstraints.size(); ++i)
	{
		ContactPositionConstraint* pc = &positionConstraints[i];

		int indexA = pc->indexA;
		int indexB = pc->indexB;
		glm::vec3 localCenterA = pc->localCenterA;
		float mA = pc->invMassA;
		glm::mat3 iA = pc->invIA;
		glm::vec3 localCenterB = pc->localCenterB;
		float mB = pc->invMassB;
		glm::mat3 iB = pc->invIB;
		int pointCount = pc->pointCount;

		glm::vec3 cA = (*positions)[indexA].c;
		glm::quat qA = (*positions)[indexA].q;

		glm::vec3 cB = (*positions)[indexB].c;
		glm::quat qB = (*positions)[indexB].q;

		// Solve normal constraints
		for (int j = 0; j < pointCount; ++j)
		{
			Transform txA, txB;
			txA.R = glm::toMat3(qA);
			txB.R = glm::toMat3(qB);
			txA.position = cA - (txA.R * localCenterA);
			txB.position = cB - (txB.R * localCenterB);

			b2PositionSolverManifold psm;
			psm.Initialize(pc, txA, txB, j);
			glm::vec3 normal = psm.normal;

			glm::vec3 point = psm.point;
			float separation = psm.separation;

			glm::vec3 rA = point - cA;
			glm::vec3 rB = point - cB;

			// Track max constraint error.
			minSeparation = glm::min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			float C = glm::clamp(baumgarte * (separation + linearSlop), -maxLinearCorrection, 0.0f);

			// Compute the effective mass.
			glm::vec3 rnA = glm::cross(rA, normal);
			glm::vec3 rnB = glm::cross(rB, normal);
			float K = mA + mB + glm::dot(iA * rnA, rnA) + glm::dot(iB * rnB, rnB);

			// Compute normal impulse
			float impulse = K > 0.0f ? -C / K : 0.0f;

			glm::vec3 P = impulse * normal;

			cA -= mA * P;
			glm::vec3 w = iA * glm::cross(rA, P);
			qA -= 0.5f * glm::quat(0.0f, w) * qA;

			cB += mB * P;
			w = iB * glm::cross(rB, P);
			qB += 0.5f * glm::quat(0.0f, w) * qB;
		}

		(*positions)[indexA].c = cA;
		(*positions)[indexA].q = qA;
		(*positions)[indexB].c = cB;
		(*positions)[indexB].q = qB;
	}

	// We can't expect minSpeparation >= -b2_linearSlop because we don't
	// push the separation above -b2_linearSlop.
	return minSeparation >= -3.0f * linearSlop;
}
