#pragma once

#include "Collider.h"

class SphereCollider;
class HullCollider;
class Manifold;

typedef Contact* ContactCreateFn(Collider* colliderA, Collider* colliderB);

typedef void ContactDestroyFn(Contact* contact);

struct CollisionTable
{
	ContactCreateFn* createFn;
	ContactDestroyFn* destroyFn;
	bool flip;
};

class NarrowPhase
{
public:
	static void AddType(ContactCreateFn* createFn, ContactDestroyFn* destroyFn, Collider::Type typeA, Collider::Type typeB);

	static void InitializeTable();

	static CollisionTable contactTable[Collider::nShapes][Collider::nShapes];
};

////////////////////////////////
////////////  GJK  ////////////    
//////////////////////////////
struct DistanceProxy
{
	DistanceProxy() {};
	DistanceProxy(Collider* collider);

	int GetSupport(const glm::vec3& dir) const;

	std::vector<glm::vec3> vertices;
};

struct DistanceInput
{
	DistanceProxy proxyA;
	DistanceProxy proxyB;
	Transform txA;
	Transform txB;
};

struct DistanceOutput
{
	glm::vec3 CA;
	glm::vec3 CB;
	float distance;
};

struct SimplexVertex
{
	glm::vec3 supportA;	// support on proxyA
	glm::vec3 supportB;	// support on proxyB
	glm::vec3 support;	// support on CSO A-B
	float weight;		// barycentric coordinate for closest point
	int indexA;			// index of supportA 
	int indexB;			// index of supportB
};

// A generic simplex (can be point, line, triangle, or tetrahedron)
struct Simplex
{
	// Find a search direction to evolve the simplex
	const glm::vec3 FindSearchDirection() const;

	// Find closest point on the simplex to the origin
	const glm::vec3 FindClosestPoint() const;

	// Calculate closest points on the colliders, using the barycentric weights of simplex vertices
	void CalculateClosestPoints(glm::vec3* pA, glm::vec3* pB);

	// Calculate the barycentric weights of the closest point on simplex to origin
	// Find the Voronoi region of the origin, and reduce the simplex if required
	void Solve2();	// Simplex is a segment
	void Solve3();	// Simplex is a triangle
	void Solve4();	// Simplex is a tetrahedron

	SimplexVertex A;
	SimplexVertex B;
	SimplexVertex C;
	SimplexVertex D;
	int nVerts;	// number of vertices on the simplex (1 - 4)
};

void GJK_Distance(const DistanceInput* input, DistanceOutput* output);
////////////////////////////////
////////////  GJK  ////////////    
//////////////////////////////

void CollideSpheres(Manifold* manifold, SphereCollider* sphereA, SphereCollider* sphereB);

void CollideHulls(Manifold* manifold, HullCollider* hullA, HullCollider* hullB,
								      const Transform& txA, const Transform& txB);

void CollideSphereHull(Manifold* manifold, SphereCollider* sphereA, HullCollider* hullB,
										   const Transform& txA, const Transform& txB);