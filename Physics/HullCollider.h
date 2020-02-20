#pragma once

#include <vector>
#include "Collider.h"
#include "../Components/Transform.h"

struct Edge;
struct Face;

struct Vertex
{
	glm::vec3 position;								
};

struct Edge
{
	Vertex* tail;			
	Face* face;			
	Edge* next;			
	Edge* twin;			

	int id;	// temp - TODO: make Edge, Face and Vertex more compact

	glm::vec3 GetDirection() const;

	Edge* Prev();
};

struct Face
{
	Edge* edge;			
	glm::vec3 normal;		

	void CalculateNormal();
};

struct AABB;
class HullCollider : public Collider
{
public:
	HullCollider();

	~HullCollider();

	void ComputeAABB(AABB* aabb) const override;

	void ComputeMass() override;

	void Scale(const glm::vec3& s);

	const glm::vec3 GetSupport(const glm::vec3& dir) const;

	std::vector<Vertex*> vertices;
	std::vector<Edge*> edges;
	std::vector<Face*> faces;

	Transform txB;	// local to body space
};