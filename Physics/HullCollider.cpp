#include "HullCollider.h"
#include "Collision.h"
#include "../Components/Body.h"

glm::vec3 Edge::GetDirection() const
{
	return next->tail->position - tail->position;
}

Edge* Edge::Prev()
{
	int n = 0;
	Edge* e = face->edge;
	do {
		n++;
		e = e->next;
	} while (e != face->edge);

	Edge* prev = this;
	for (int i = 1; i < n; i++)
		prev = prev->next;

	return prev;
}

void Face::CalculateNormal()
{
	normal = glm::cross(edge->GetDirection(), edge->next->GetDirection());
	normal = glm::normalize(normal);
}

HullCollider::HullCollider()
{
	type = ConvexHull;
	density = 1.0f;
	txB.position = glm::vec3(0.0f);
	txB.R = glm::mat3(1.0f);
}

Collider* HullCollider::Clone() const
{
	return (new HullCollider());
}

void HullCollider::Scale(const glm::vec3& s)
{
	for (Vertex* v : vertices)
	{
		v->position.x *= s.x;
		v->position.y *= s.y;
		v->position.z *= s.z;
	}
}

void HullCollider::ComputeAABB(AABB* aabb) const
{
	Transform txW;
	txW.R = body->tx.R * txB.R;
	txW.position = body->tx.R * txB.position + body->tx.position;
	glm::vec3 min = txW.R * vertices[0]->position + txW.position;
	glm::vec3 max = min;
	int N = vertices.size();

	for (int i = 1; i < N; ++i)
	{
		glm::vec3 v = txW.R * vertices[i]->position + txW.position;
		min = glm::vec3(glm::min(min.x, v.x), glm::min(min.y, v.y), glm::min(min.z, v.z));
		max = glm::vec3(glm::max(max.x, v.x), glm::max(max.y, v.y), glm::max(max.z, v.z));
	}

	aabb->min = min;
	aabb->max = max;
}

void HullCollider::ComputeMass()
{
	glm::vec3 diag(0.0f);
	glm::vec3 offDiag(0.0f);
	float volume = 0.0f;
	com = glm::vec3(0.0f);
	mass = 0.0f;
	inertia = glm::mat3(0.0f);

	for (int i = 0; i < faces.size(); i++)
	{
		auto start = faces[i]->edge;
		auto middle = start->next;
		auto last = middle->next;

		while (start != last)
		{
			glm::vec3 u = start->tail->position;
			glm::vec3 v = middle->tail->position;
			glm::vec3 w = last->tail->position;

			float currentVolume = glm::dot(u, glm::cross(v, w));
			volume += currentVolume;
			com += (u + v + w) * currentVolume;

			for (int j = 0; j < 3; ++j)
			{
				int j1 = (j + 1) % 3;
				int j2 = (j + 2) % 3;

				diag[j] += (
					u[j] * v[j] + v[j] * w[j] + w[j] * u[j] +
					u[j] * u[j] + v[j] * v[j] + w[j] * w[j]) * currentVolume;

				offDiag[j] += (
					u[j1] * v[j2] + v[j1] * w[j2] + w[j1] * u[j2] +
					u[j1] * w[j2] + v[j1] * u[j2] + w[j1] * v[j2] +
					u[j1] * u[j2] * 2.0f + v[j1] * v[j2] * 2.0f + w[j1] * w[j2] * 2.0f) * currentVolume;
			}

			middle = last;
			last = last->next;
		}
	}

	com /= (volume * 4.0f);
	com = txB.position + (txB.R * com);

	volume *= (1.0f / 6.0f);
	diag /= volume * 60.0f;
	offDiag /= volume * 120.0f;
	mass = density * volume;

	inertia[0] = glm::vec3(diag.y + diag.z, -offDiag.z,		 -offDiag.y);
	inertia[1] = glm::vec3(-offDiag.z,		diag.x + diag.z, -offDiag.x);
	inertia[2] = glm::vec3(-offDiag.y,		-offDiag.x,		 diag.x + diag.y);

	inertia *= mass;
}

HullCollider::~HullCollider()
{
	for (Vertex* v : vertices)
		delete v;
	for (Edge* e : edges)
		delete e;
	for (Face* f : faces)
		delete f;

	vertices.clear();
	edges.clear();
	faces.clear();
}