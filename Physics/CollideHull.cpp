
#include "NarrowPhase.h"
#include "Contact.h"
#include "HullCollider.h"
#include "Settings.h"
#include "../Mesh/Geometry.h"

struct FaceQuery
{
	float separation;
	int index;
};

void QueryFaceAxes(FaceQuery* query, const HullCollider* hullA, const HullCollider* hullB, const Transform& bLocal)
{
	int maxIndex = 0;
	float maxSep = -FLT_MAX;
	int N = hullA->faces.size();

	for (int i = 0; i < N; ++i)
	{
		Face* face = hullA->faces[i];
		glm::vec3 n = bLocal.R * (face->normal);
		glm::vec3 p = bLocal.R * (face->edge->tail->position) + bLocal.position;

		glm::vec3 support = hullB->GetSupport(-n);
		float separation = glm::dot((support - p), n);

		// TODO: possible early out? Box2D and Rubicon dont check this

		if (separation > maxSep)
		{
			maxIndex = i;
			maxSep = separation;
		}
	}

	query->index = maxIndex;
	query->separation = maxSep;
}

int FindIncidentFace(HullCollider* incident, HullCollider* reference, int referenceFace, const Transform& txI)
{
	float minDot = FLT_MAX;
	int index = -1;
	glm::vec3 n = reference->faces[referenceFace]->normal;
	n = txI.R * n;

	int N = incident->faces.size();
	for (int i = 0; i < N; ++i)
	{
		float dot = glm::dot(incident->faces[i]->normal, n);
		if (dot < minDot)
		{
			minDot = dot;
			index = i;
		}
	}

	return index;
}

void ClipFaces(Manifold* m, HullCollider* inc, HullCollider* ref, int incFace, int refFace,
							const Transform& txI, const Transform& txR)
{
	// the incident face to be clipped
	std::vector<glm::vec3> inPoly;	//inPoly.reserve(4);
	Edge* start = inc->faces[incFace]->edge;
	Edge* edge = start;
	do {	
		glm::vec3 v = edge->tail->position;
		v = txI.R * v + txI.position;
		inPoly.push_back(v);
		edge = edge->next;
	} while (edge != start);

	// the clipping planes formed by extruding faces adjacent to reference face
	std::vector<HalfSpace> planes;	//planes.reserve(4);
	start = ref->faces[refFace]->edge;
	edge = start;
	do {
		glm::vec3 n = edge->twin->face->normal;
		n = txR.R * n;
		glm::vec3 p = edge->tail->position;
		p = txR.R * p + txR.position;
		planes.push_back(HalfSpace(n, p));
		edge = edge->next;
	} while (edge != start);

	std::vector<glm::vec3> outPoly;	//outPoly.reserve(4);

	for (HalfSpace plane : planes)
	{
		glm::vec3 A = inPoly[inPoly.size() - 1];
		for (int i = 0; i < inPoly.size(); ++i)
		{
			glm::vec3 B = inPoly[i];
			float dA = glm::dot(A, plane.normal) - plane.distance;
			float dB = glm::dot(B, plane.normal) - plane.distance;

			// A and B Behind or On plane - out B
			if ((dA < 0.0f && dB < 0.0f) || (glm::abs(dA) < fxEpsilon) || (glm::abs(dB) < fxEpsilon))
			{
				outPoly.push_back(B);
			}
			// A Infront and B Behind - out I, B
			else if (dA > 0.0f && dB < 0.0f)
			{
				outPoly.push_back(A + (dA / (dA - dB)) * (B - A));
				outPoly.push_back(B);
			}
			// A Behind and B Infront - out I
			else if (dA < 0.0f && dB > 0.0f)
			{
				outPoly.push_back(A + (dA / (dA - dB)) * (B - A));
			}
			A = B;
		}

		if (outPoly.size() == 0)	// why is this happening?
		{
			m->nPoints = 0;
			return;
		}

		inPoly = outPoly;
		outPoly.clear();
	}

	glm::vec3 n = txR.R * ref->faces[refFace]->normal;
	glm::vec3 p = txR.R * (ref->faces[refFace]->edge->tail->position) + txR.position;
	for (glm::vec3& v : inPoly)
	{
		float separation = glm::dot(v - p, n);
		if (separation < hullRadiusSum)
			outPoly.push_back(v);
	}

	// reduce to max 4
	while (outPoly.size() > 4)
	{
		int x = 1;
		outPoly.pop_back();
	}

	m->nPoints = outPoly.size();
	m->localNormal = ref->txB.R * (ref->faces[refFace]->normal);
	m->localPoint = ref->txB.R * (ref->faces[refFace]->edge->tail->position) + ref->txB.position;

	for (int i = 0; i < m->nPoints; ++i)
	{
		m->points[i].localPoint = glm::transpose(txI.R) * (outPoly[i] - txI.position);
	}
}

void CollideHulls(Manifold* manifold, HullCollider* hullA, HullCollider* hullB,
									  const Transform& txA, const Transform& txB)
{
	manifold->nPoints = 0;
	Transform atx, btx, bLocal;
	atx = FxMul(txA, hullA->txB);
	btx = FxMul(txB, hullB->txB);
	bLocal = FxMulT(btx, atx);

	FaceQuery queryA;
	QueryFaceAxes(&queryA, hullA, hullB, bLocal);
	if (queryA.separation > hullRadiusSum)
		return;

	Transform aLocal;
	aLocal = FxMulT(atx, btx);

	FaceQuery queryB;
	QueryFaceAxes(&queryB, hullB, hullA, aLocal);
	if (queryB.separation > hullRadiusSum)
		return;

	// edge-edge

	HullCollider* incident;
	HullCollider* reference;
	Transform txL, txI, txR;
	bool flip;
	int incidentFace, referenceFace;

	if (queryA.separation > queryB.separation + linearSlop)
	{
		incident = hullB;
		reference = hullA;
		referenceFace = queryA.index;
		txL = bLocal;
		txI = btx;
		txR = atx;
		manifold->type = Manifold::faceA;
		flip = false;
	}
	else
	{
		incident = hullA;
		reference = hullB;
		referenceFace = queryB.index;
		txL = aLocal;
		txI = atx;
		txR = btx;
		manifold->type = Manifold::faceB;
		flip = true;
	}

	incidentFace = FindIncidentFace(incident, reference, referenceFace, txL);

	ClipFaces(manifold, incident, reference, incidentFace, referenceFace, txI, txR);
}