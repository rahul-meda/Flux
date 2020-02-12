
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

			//glm::vec3 tangent[2];
			//ComputeBasis(vc->normal, &tangent[0], &tangent[1]);

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
			vcp->velocityBias += baumgarte * 60.0f * glm::max(-worldManifold.separations[0] - linearSlop, 0.0f);
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
		//glm::vec3 tangent[2];
		//ComputeBasis(vc->normal, &tangent[0], &tangent[1]);

		for (int j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;
			glm::vec3 P = vcp->normalImpulse * normal + vcp->tangentImpulse[0] * vc->tangent[0];
			P += vcp->tangentImpulse[1] * vc->tangent[1];
			//P *= 0.3f;
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
		//glm::vec3 tangent[2];
		//ComputeBasis(normal, &tangent[0], &tangent[1]);
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
			float newImpulse = glm::max(vcp->normalImpulse + lambda, 0.0f);
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