
#include "NarrowPhase.h"
#include "Contact.h"
#include "HullCollider.h"
#include "Settings.h"
#include "../Mesh/Geometry.h"
#include <iostream>

struct FaceQuery
{
	float separation;
	int index;
};

struct EdgeQuery
{
	glm::vec3 normal;
	float separation;
	int indexA;
	int indexB;
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

		// TODO: possible early out?

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

struct ClipVertex
{
	glm::vec3 v;
	ContactID id;
};

// choose 4 points that maximize the area of the manifold
void ReduceManifold(std::vector<ClipVertex>& inPoly, glm::vec3& n)
{
	std::vector<ClipVertex> outPoly;	outPoly.reserve(4);

	// A. find a support point 
	float madMax = -FLT_MAX;
	int index = -1;
	int N = inPoly.size();
	for (int i = 0; i < N; ++i)
	{
		float dot = glm::dot(inPoly[i].v, glm::vec3(1.0f, 0.0f, 0.0f));
		if (dot > madMax)
		{
			madMax = dot;
			index = i;
		}
	}

	outPoly.push_back(inPoly[index]);
	inPoly[index] = inPoly[--N];
	inPoly.pop_back();

	// B. furthest point from A 
	madMax = -FLT_MAX;
	index = -1;
	glm::vec3 A = outPoly[0].v;
	for (int i = 0; i < N; ++i)
	{
		float dist = glm::length2(A - inPoly[i].v);
		if (dist > madMax)
		{
			madMax = dist;
			index = i;
		}
	}
	outPoly.push_back(inPoly[index]);
	inPoly[index] = inPoly[--N];
	inPoly.pop_back();

	// C. maximize area of ABC
	madMax = -FLT_MAX;
	index = -1;
	glm::vec3 B = outPoly[1].v;
	bool ccw = true;
	for (int i = 0; i < N; ++i)
	{
		glm::vec3 P = inPoly[i].v;
		float area = glm::dot(glm::cross(P - A, P - B), n);
		if (area < 0)
		{
			area = -area;
			ccw = false;
		}
		else
		{
			ccw = true;
		}
		if (area > madMax)
		{
			madMax = area;
			index = i;
		}
	}
	outPoly.push_back(inPoly[index]);
	if (!ccw)
	{
		ClipVertex temp = outPoly[1];
		outPoly[1] = outPoly[2];
		outPoly[2] = temp;
	}
	inPoly[index] = inPoly[--N];
	inPoly.pop_back();

	// D. maximize area of ABCD
	madMax = -FLT_MAX;
	index = -1;
	for (int j = 0; j < 3; ++j)
	{
		A = outPoly[j].v;
		B = outPoly[(j + 4) % 3].v;
		for (int i = 0; i < N; ++i)
		{
			glm::vec3 P = inPoly[i].v;
			float area = glm::dot(glm::cross(P - A, P - B), n);
			if (area < 0)
			{
				/*area = -area;
				ccw = false;*/
				continue;
			}
			/*else
			{
				ccw = true;
				continue;
			}*/
			if (area > madMax)
			{
				madMax = area;
				index = i;
			}
		}
	}
	if (index >= 0)
		outPoly.push_back(inPoly[index]);

	inPoly.clear();		inPoly.reserve(4);
	for (int i = 0; i < outPoly.size(); ++i)
		inPoly.push_back(outPoly[i]);
}

void ClipFaces(Manifold* m, HullCollider* inc, HullCollider* ref, int incFace, int refFace,
							const Transform& txI, const Transform& txR)
{
	// the incident face to be clipped
	std::vector<ClipVertex> inPoly;		inPoly.reserve(4);
	Edge* start = inc->faces[incFace]->edge;
	Edge* edge = start;
	do {	
		ClipVertex cv;
		cv.v = txI.R * edge->tail->position + txI.position;
		cv.id.featurePair.edgeA = char(edge->id);
		cv.id.featurePair.faceA = char(incFace);
		cv.id.featurePair.edgeB = char(edge->next->id);
		cv.id.featurePair.faceB = char(incFace);
		inPoly.push_back(cv);
		edge = edge->next;
	} while (edge != start);

	// the clipping planes formed by extruding faces adjacent to reference face
	std::vector<HalfSpace> planes;		planes.reserve(4);
	std::vector<int> refEdgeIDs;		refEdgeIDs.reserve(4);
	start = ref->faces[refFace]->edge;
	edge = start;
	do {
		glm::vec3 n = edge->twin->face->normal;
		n = txR.R * n;
		glm::vec3 p = edge->tail->position;
		p = txR.R * p + txR.position;
		planes.push_back(HalfSpace(n, p));
		refEdgeIDs.push_back(edge->id);
		edge = edge->next;
	} while (edge != start);

	std::vector<ClipVertex> outPoly;		outPoly.reserve(4);

	int NP = planes.size();
	for (int iP = 0; iP < NP; ++iP)
	{
		glm::vec3 A = inPoly[inPoly.size() - 1].v;
		for (int i = 0; i < inPoly.size(); ++i)
		{
			glm::vec3 B = inPoly[i].v;
			ContactID cid = inPoly[i].id;
			
			HalfSpace plane = planes[iP];
			float dA = glm::dot(A, plane.normal) - plane.distance;
			float dB = glm::dot(B, plane.normal) - plane.distance;

			ClipVertex cv;

			// A and B Behind or On plane - out B
			if ((dA < 0.0f && dB < 0.0f) || (glm::abs(dA) < fxEpsilon) || (glm::abs(dB) < fxEpsilon))
			{
				cv.v = B;
				cv.id = cid;
				outPoly.push_back(cv);
			}
			// A Infront and B Behind - out I, B
			else if (dA > 0.0f && dB < 0.0f)
			{
				cv.v = A + (dA / (dA - dB)) * (B - A);
				cv.id.featurePair.edgeA = char(refEdgeIDs[iP]);
				cv.id.featurePair.faceA = char(refFace);
				cv.id.featurePair.edgeB = char(cid.featurePair.edgeA);
				cv.id.featurePair.faceB = char(cid.featurePair.faceA);
				outPoly.push_back(cv);

				cv.v = B;
				cv.id = cid;
				outPoly.push_back(cv);
			}
			// A Behind and B Infront - out I
			else if (dA < 0.0f && dB > 0.0f)
			{
				cv.v = A + (dA / (dA - dB)) * (B - A);
				cv.id.featurePair.edgeA = char(cid.featurePair.edgeA);
				cv.id.featurePair.faceA = char(cid.featurePair.faceA);
				cv.id.featurePair.edgeB = char(refEdgeIDs[iP]);
				cv.id.featurePair.faceB = char(refFace);
				outPoly.push_back(cv);
			}
			A = B;
		}

		if (outPoly.size() == 0)	// why is this happening?
		{
			//assert(false, "ERROR::Invalid clipping points");
			m->nPoints = 0;
			return;
		}

		inPoly = outPoly;
		outPoly.clear();
	}

	glm::vec3 n = txR.R * ref->faces[refFace]->normal;
	glm::vec3 p = txR.R * (ref->faces[refFace]->edge->tail->position) + txR.position;
	for (ClipVertex& cv : inPoly)
	{
		float separation = glm::dot(cv.v - p, n);
		if (separation < hullRadiusSum)
			outPoly.push_back(cv);
	}

	// reduce to max 4
	glm::vec3 normal = txI.R * inc->faces[incFace]->normal;
	while (outPoly.size() > 4)
	{
		ReduceManifold(outPoly, normal);
	}

	m->nPoints = outPoly.size();
	m->localNormal = ref->txB.R * (ref->faces[refFace]->normal);
	m->localPoint = ref->txB.R * (ref->faces[refFace]->edge->tail->position) + ref->txB.position;

	for (int i = 0; i < m->nPoints; ++i)
	{
		m->points[i].localPoint = outPoly[i].v; 
		m->points[i].id = outPoly[i].id;
	}
}

inline bool IsMinkowskiFace(const glm::vec3& A, const glm::vec3& B, const glm::vec3& B_x_A,
							const glm::vec3& C, const glm::vec3& D, const glm::vec3& D_x_C)
{
	// Test if points A B C and D form intersecting arcs upon the overlayed Gauss map
	float CBA = glm::dot(C, B_x_A);
	float DBA = glm::dot(D, B_x_A);
	float ADC = glm::dot(A, D_x_C);
	float BDC = glm::dot(B, D_x_C);

	return ( (CBA * DBA < 0.0f)		// Intersection test
		   &&(ADC * BDC < 0.0f)		// Intersection test
		   &&(CBA * BDC > 0.0f) );	// Hemisphere test
}

void QueryEdgeAxes(EdgeQuery* query, const HullCollider* A, const HullCollider* B, const Transform& bLocal)
{
	glm::vec3 bestAxis(0.0f);
	float maxSep = -FLT_MAX;
	int bestIndexA = 0;
	int bestIndexB = 0;

	glm::vec3 CA = bLocal.R * A->com + bLocal.position;

	int NA = A->edges.size() / 2;
	for (int iA = 0; iA < NA; ++iA)
	{
		Edge* edgeA = A->edges[iA];
		Edge* twinA = edgeA->twin;	// TODO: check if sorting twins properly improves cache
		glm::vec3 PA = bLocal.R * edgeA->tail->position + bLocal.position;
		glm::vec3 QA = bLocal.R * twinA->tail->position + bLocal.position;
		glm::vec3 EA = QA - PA;

		glm::vec3 UA = bLocal.R * edgeA->face->normal;
		glm::vec3 VA = bLocal.R * twinA->face->normal;

		int NB = B->edges.size() / 2;
		for (int iB = 0; iB < NB; ++iB)
		{
			Edge* edgeB = B->edges[iB];
			Edge* twinB = edgeB->twin;	
			glm::vec3 PB = edgeB->tail->position;
			glm::vec3 QB = twinB->tail->position;
			glm::vec3 EB = QB - PB;

			glm::vec3 UB = edgeB->face->normal;
			glm::vec3 VB = twinB->face->normal;

			if (IsMinkowskiFace(UA, VA, -EA, -UB, -VB, -EB))
			{
				glm::vec3 axis = glm::cross(EA, EB);
				
				float l2 = glm::length2(axis);
				if (l2 < linearSlop2)	// skip parallel edges	// todo: fails when edge lengths are very small
				{
					continue;
				}

				axis = glm::normalize(axis);

				if (glm::dot(PA - CA, axis) < 0.0f)
					axis = -axis;

				float sep = glm::dot(PB -PA, axis);
				
				if (sep > maxSep)
				{
					bestAxis = axis;
					maxSep = sep;
					bestIndexA = iA;
					bestIndexB = iB;
				}
			}
		}
	}

	query->normal = bestAxis;
	query->separation = maxSep;
	query->indexA = bestIndexA;
	query->indexB = bestIndexB;
}

void IntersectEdges(glm::vec3* CA, glm::vec3* CB, const glm::vec3& PA, const glm::vec3& QA, 
												  const glm::vec3& PB, const glm::vec3& QB)
{
	glm::vec3 DA = QA - PA;
	glm::vec3 DB = QB - PB;
	glm::vec3 r = PA - PB;
	float a = glm::dot(DA, DA);
	float e = glm::dot(DB, DB);
	float f = glm::dot(DB, r);
	float c = glm::dot(DA, r);

	float b = glm::dot(DA, DB);
	float d = a * e - b * b;

	if (d <= 0.0f)
		return;

	assert(d > 0.0f, "ERROR::Invalid edge intersection");

	const static float eTol = 0.02;
	float TA = (b * f - c * e) / d;
	//assert(TA >= 0.0f - eTol && TA <= 1.0f + eTol, "ERROR::Invalid edge intersection");
	float TB = (b * TA + f) / e;
	//assert(TB >= 0.0f - eTol && TB <= 1.0f + eTol, "ERROR::Invalid edge intersection");

	*CA = PA + DA * TA;
	*CB = PB + DB * TB;
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

	EdgeQuery edgeQuery;
	QueryEdgeAxes(&edgeQuery, hullA, hullB, bLocal);
	if (edgeQuery.separation > hullRadiusSum)
		return;

	float maxFaceSep = glm::max(queryA.separation, queryB.separation);
	if (edgeQuery.separation > maxFaceSep + linearSlop)
	{
		Edge* edgeA = hullA->edges[edgeQuery.indexA];
		glm::vec3 PA = atx.R * edgeA->tail->position + atx.position;
		glm::vec3 QA = atx.R * edgeA->twin->tail->position + atx.position;

		Edge* edgeB = hullB->edges[edgeQuery.indexB];
		glm::vec3 PB = btx.R * edgeB->tail->position + btx.position;
		glm::vec3 QB = btx.R * edgeB->twin->tail->position + btx.position;

		glm::vec3 CA, CB;
		IntersectEdges(&CA, &CB, PA, QA, PB, QB);

		ContactID id;
		id.featurePair.edgeA = edgeQuery.indexA;
		id.featurePair.faceA = 0;
		id.featurePair.edgeB = edgeQuery.indexB;
		id.featurePair.faceB = 0;

		manifold->type = Manifold::edges;
		manifold->localPoint = glm::transpose(txA.R) * (CA - txA.position);
		manifold->localNormal = edgeQuery.normal;
		manifold->nPoints = 1;
		manifold->points[0].localPoint = glm::transpose(txB.R) * (CB - txB.position);
		manifold->points[0].id = id;
	}
	else
	{
		HullCollider* incident;
		HullCollider* reference;
		Transform txL, txI, txR, txBody;
		int incidentFace, referenceFace;

		if (queryA.separation > queryB.separation + linearSlop)
		{
			incident = hullB;
			reference = hullA;
			referenceFace = queryA.index;
			txL = bLocal;
			txI = btx;
			txR = atx;
			txBody = txB;
			manifold->type = Manifold::faceA;
		}
		else
		{
			incident = hullA;
			reference = hullB;
			referenceFace = queryB.index;
			txL = aLocal;
			txI = atx;
			txR = btx;
			txBody = txA;
			manifold->type = Manifold::faceB;
		}

		incidentFace = FindIncidentFace(incident, reference, referenceFace, txL);

		ClipFaces(manifold, incident, reference, incidentFace, referenceFace, txI, txR);

		for (int i = 0; i < manifold->nPoints; ++i)
		{
			glm::vec3 p = manifold->points[i].localPoint;
			
			manifold->points[i].localPoint = glm::transpose(txBody.R) * (p - txBody.position);
		}
	}
}