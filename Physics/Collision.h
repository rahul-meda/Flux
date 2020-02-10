#pragma once

#include <glm/glm.hpp>
#include "Settings.h"
#include "../Components/Transform.h"

class Collider;

struct AABB
{
	AABB()
	{}

	AABB(const glm::vec3& min, const glm::vec3& max, Collider* c)
		: min(min), max(max), collider(c)
	{}

	glm::vec3 min;
	glm::vec3 max;

	Collider* collider;
};

bool Overlap(AABB* A, AABB* B);

////////////////////////////////////////////////////////////////////////////////////////
// Contact caching for warmstarting
// Allows faster convergence of iterative solver - Ref: Box2D
// Each contact point is uniquely identified from the features generating the contact
////////////////////////////////////////////////////////////////////////////////////////
struct ContactFeature
{
	unsigned char edgeA;	// can't have more than 255 edges!!
	unsigned char faceA;
	unsigned char edgeB;
	unsigned char faceB;
};

union ContactID
{
	ContactFeature featurePair;
	unsigned int key;
};

struct ManifoldPoint
{
	glm::vec3 localPoint;
	glm::vec3 normalImpulse;
	glm::vec3 tangentImpulse;
	ContactID id;
};

struct Manifold
{
	enum Type
	{
		spheres,
		faceA,
		faceB
	};

	ManifoldPoint points[maxManifoldPoints];
	glm::vec3 localNormal;
	glm::vec3 localPoint;
	Type type;
	int nPoints;
};

struct WorldManifold
{
	void Initialize(const Manifold* manifold, const Transform& txA, const Transform& txB, 
											  float radiusA, float radiusB);

	glm::vec3 normal;
	glm::vec3 tangent[2];
	glm::vec3 points[maxManifoldPoints];
	float separations[maxManifoldPoints];
};

enum PointState
{
	invalid,		// point doesn't exist
	newlyAdded,		// point was added in the update
	persisted,		// point persisted accross update
	removed			// point was removed in the update
};

// The states pertain to transition from m1 to m2
// State1 is either persist or remove, and state2 is either added or persist 
void GetPointStates(PointState state1[maxManifoldPoints], PointState state2[maxManifoldPoints],
					const Manifold* m1, const Manifold* m2);