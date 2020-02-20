#include "Mesh.h"
#include "../Components/Model.h"
#include "../Physics/HullCollider.h"

glm::vec3 HEdge::GetDirection() const
{
	return next->tail->position - tail->position;
}

HEdge* HEdge::Prev()
{
	int n = 0;
	HEdge* e = face->edge;
	do {
		n++;
		e = e->next;
	} while (e != face->edge);

	HEdge* prev = this;
	for (int i = 1; i < n; i++)
		prev = prev->next;

	return prev;
}

void HFace::CalculateNormal()
{
	normal = glm::cross(edge->GetDirection(), edge->next->GetDirection());
	normal = glm::normalize(normal);
}

void HMesh::Scale(const glm::vec3& scale)
{
	for (auto v : vertices)
	{
		v->position.x *= scale.x;
		v->position.y *= scale.y;
		v->position.z *= scale.z;
	}
}

void HMesh::GetModelData(ModelDef& md) const
{
	for (HFace* f : faces)
	{
		HEdge* start = f->edge;
		HEdge* e = start;
		glm::vec3 first, second, third;
		first = e->tail->position;
		do		// triangle fan
		{
			second = e->next->tail->position;
			e = e->next;
			third = e->next->tail->position;

			md.vertices.push_back(first);
			md.normals.push_back(f->normal);
			md.vertices.push_back(second);
			md.normals.push_back(f->normal);
			md.vertices.push_back(third);
			md.normals.push_back(f->normal);
		} while (e->next->next != start);
	}
}

void HMesh::GetColliderData(HullCollider* c) const
{
	c->vertices = std::vector<Vertex*>(vertices.size(), nullptr);
	c->edges = std::vector<Edge*>(edges.size(), nullptr);
	c->faces = std::vector<Face*>(faces.size(), nullptr);

	int NV = vertices.size();
	int NE = edges.size();
	int NF = faces.size();
	for (int i = 0; i < NV; ++i)
	{
		Vertex *v = new Vertex();
		v->position = vertices[i]->position;
		c->vertices[vertices[i]->id - 1] = v;
	}

	for (int i = 0; i < NF; ++i)
	{
		Face *f = new Face();
		f->normal = faces[i]->normal;
		f->edge = nullptr;
		c->faces[faces[i]->id - 1] = f;
	}

	for (int i = 0; i < NE; ++i)
	{
		Edge *e = new Edge();
		e->tail = c->vertices[edges[i]->tail->id - 1];
		e->face = c->faces[edges[i]->face->id - 1];
		e->id = i;
		c->edges[edges[i]->id - 1] = e;
	}

	for (int i = 0; i < NE; ++i)
	{
		c->edges[edges[i]->id - 1]->next = c->edges[edges[i]->next->id - 1];
		if (edges[i]->twin != nullptr) 
		{
			c->edges[edges[i]->id - 1]->twin = c->edges[edges[i]->twin->id - 1];
		}
	}

	for (int i = 0; i < NF; ++i)
	{
		c->faces[faces[i]->id - 1]->edge = c->edges[faces[i]->edge->id - 1];
		c->faces[faces[i]->id - 1]->CalculateNormal();
	}
}

void HMesh::Cleanup()
{
	for (auto v : vertices)
		delete v;
	for (auto e : edges)
		delete e;
	for (auto f : faces)
		delete f;

	vertices.clear();
	edges.clear();
	faces.clear();
}

HMesh::~HMesh()
{
	Cleanup();
}