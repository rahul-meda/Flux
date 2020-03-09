#pragma once

#include <glm/glm.hpp>
#include "Settings.h"
#include "../Components/Transform.h"

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
	float normalImpulse;
	float tangentImpulse[2];
	ContactID id;
};

struct Manifold
{
	enum Type
	{
		sphere,
		faceA,
		faceB,
		edges
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