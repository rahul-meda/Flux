
#include "NarrowPhase.h"
#include "SphereCollider.h"
#include "HullCollider.h"
#include "CapsuleCollider.h"

DistanceProxy::DistanceProxy(Collider* collider)
{
	switch (collider->type)
	{
		case Collider::Sphere:
		{
			SphereCollider* sc = (SphereCollider*)collider;
			vertices.push_back(sc->com);
			break;
		}
		case Collider::ConvexHull:
		{
			HullCollider* hc = (HullCollider*)collider;
			int nVerts = hc->vertices.size();
			vertices.reserve(nVerts);
			for (int i = 0; i < nVerts; ++i)
				vertices.push_back(hc->vertices[i]->position);
			break;
		}
		case Collider::Capsule:
		{
			CapsuleCollider* c = (CapsuleCollider*)collider;
			glm::vec3 C = c->com;
			float h = c->H;
			glm::vec3 d = h * c->upB;
			vertices.push_back(C - d);
			vertices.push_back(C + d);
		}
	}
}

inline int DistanceProxy::GetSupport(const glm::vec3& d) const
{
	int bestIndex = 0;
	float bestDot = glm::dot(vertices[0], d);

	int N = vertices.size();
	for (int i = 1; i < N; ++i)
	{
		float dot = glm::dot(vertices[i], d);
		if (dot > bestDot)
		{
			bestIndex = i;
			bestDot = dot;
		}
	}

	return bestIndex;
}

void Simplex::CalculateClosestPoints(glm::vec3* a, glm::vec3* b)
{
	switch (nVerts)
	{
	case 1:
		*a = A.supportA;
		*b = A.supportB;
		break;

	case 2:
		*a = (A.supportA * A.weight) + (B.supportA * B.weight);
		*b = (A.supportB * A.weight) + (B.supportB * B.weight);
		break;

	case 3:
		*a = (A.supportA * A.weight) + (B.supportA * B.weight) + (C.supportA * C.weight);
		*b = (A.supportB * A.weight) + (B.supportB * B.weight) + (C.supportB * C.weight);
		break;

	case 4:
		*a = (A.supportA * A.weight) + (B.supportA * B.weight) + (C.supportA * C.weight) +   (D.supportA * D.weight);
		*b = *a;
		break;

	default:
		assert(false);
		break;
	}
}

void Simplex::Solve2()
{
	glm::vec3 a = A.support;
	glm::vec3 b = B.support;

	float u = glm::dot(b, b - a);
	float v = glm::dot(a, a - b);

	// region a
	if (v <= 0.0f)
	{
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region b
	if (u <= 0.0f)
	{
		A = B;
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region ab
	float invDenom = 1.0f / (u + v);
	A.weight = u * invDenom;
	B.weight = v * invDenom;
	nVerts = 2;
}

void Simplex::Solve3()
{
	glm::vec3 a = A.support;
	glm::vec3 b = B.support;
	glm::vec3 c = C.support;

	float uAB = glm::dot(b, b - a);
	float vAB = glm::dot(a, a - b);
	float uBC = glm::dot(c, c - b);
	float vBC = glm::dot(b, b - c);
	float uCA = glm::dot(a, a - c);
	float vCA = glm::dot(c, c - a);

	glm::vec3 n = glm::cross(b - a, c - a);
	float uABC = glm::dot(glm::cross(b, c), n);
	float vABC = glm::dot(glm::cross(c, a), n);
	float wABC = glm::dot(glm::cross(a, b), n);

	// region a
	if (vAB <= 0.0f && uCA <= 0.0f)
	{
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region b
	if (uAB <= 0.0f && vBC <= 0.0f)
	{
		A = B;
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region c
	if (uBC <= 0.0f && vCA <= 0.0f)
	{
		A = C;
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region ab
	if (uAB > 0.0f && vAB > 0.0f && wABC <= 0.0f)
	{
		float invDenom = 1.0f / (uAB + vAB);
		A.weight = uAB * invDenom;
		B.weight = vAB * invDenom;
		nVerts = 2;
		return;
	}

	// region bc
	if (uBC > 0.0f && vBC > 0.0f && uABC <= 0.0f)
	{
		A = B;
		B = C;
		float invDenom = 1.0f / (uBC + vBC);
		A.weight = uBC * invDenom;
		B.weight = vBC * invDenom;
		nVerts = 2;
		return;
	}

	// region ca
	if (uCA > 0.0f && vCA > 0.0f && vABC <= 0.0f)
	{
		B = A;
		A = C;
		float invDenom = 1.0f / (uCA + vCA);
		A.weight = uCA * invDenom;
		B.weight = vCA * invDenom;
		nVerts = 2;
		return;
	}

	// region abc
	assert(uABC > 0.0f && vABC > 0.0f && wABC > 0.0f);
	float invDenom = 1.0f / (uABC + vABC + wABC);
	A.weight = uABC * invDenom;
	B.weight = vABC * invDenom;
	C.weight = wABC * invDenom;
	nVerts = 3;
}

void Simplex::Solve4()
{
	glm::vec3 a = A.support;
	glm::vec3 b = B.support;
	glm::vec3 c = C.support;
	glm::vec3 d = D.support;

	float uAB = glm::dot(b, b - a);
	float vAB = glm::dot(a, a - b);
	float uBC = glm::dot(c, c - b);
	float vBC = glm::dot(b, b - c);
	float uCA = glm::dot(a, a - c);
	float vCA = glm::dot(c, c - a);
	float uBD = glm::dot(d, d - b);
	float vBD = glm::dot(b, b - d);
	float uDC = glm::dot(c, c - d);
	float vDC = glm::dot(d, d - c);
	float uAD = glm::dot(d, d - a);
	float vAD = glm::dot(a, a - d);

	glm::vec3 n = glm::cross(d - a, b - a);
	float uADB = glm::dot(glm::cross(d, b), n);
	float vADB = glm::dot(glm::cross(b, a), n);
	float wADB = glm::dot(glm::cross(a, d), n);

	n = glm::cross(c - a, d - a);
	float uACD = glm::dot(glm::cross(c, d), n);
	float vACD = glm::dot(glm::cross(d, a), n);
	float wACD = glm::dot(glm::cross(a, c), n);

	n = glm::cross(b - c, d - c);
	float uCBD = glm::dot(glm::cross(b, d), n);
	float vCBD = glm::dot(glm::cross(d, c), n);
	float wCBD = glm::dot(glm::cross(c, b), n);

	n = glm::cross(b - a, c - a);
	float uABC = glm::dot(glm::cross(b, c), n);
	float vABC = glm::dot(glm::cross(c, a), n);
	float wABC = glm::dot(glm::cross(a, b), n);
	
	float denom = glm::dot(c - b, glm::cross(a - b, d - b));
	float volume = 0.0f;

	if (denom == 0.0f)
	{
		volume = 1.0f;
	}
	else
	{
		volume = 1.0f / denom;
	}

	float uABCD = glm::dot(c, glm::cross(d, b)) * volume;
	float vABCD = glm::dot(c, glm::cross(a, d)) * volume;
	float wABCD = glm::dot(d, glm::cross(a, b)) * volume;
	float xABCD = glm::dot(b, glm::cross(a, c)) * volume;

	// region a
	if (vAB <= 0.0f && uCA <= 0.0f && vAD <= 0.0f)
	{
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region b
	if (uAB <= 0.0f && vBC <= 0.0f && vBD <= 0.0f)
	{
		A = B;
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region c
	if (uBC <= 0.0f && vCA <= 0.0f && uDC <= 0.0f)
	{
		A = C;
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region d
	if (uBD <= 0.0f && vDC <= 0.0f && uAD <= 0.0f)
	{
		A = D;
		A.weight = 1.0f;
		nVerts = 1;
		return;
	}

	// region ab
	if (wABC <= 0.0f && vADB <= 0.0f && uAB > 0.0f && vAB > 0.0f)
	{
		float invDenom = 1.0f / (uAB + vAB);
		A.weight = uAB * invDenom;
		B.weight = vAB * invDenom;
		nVerts = 2;
		return;
	}

	// region bc
	if (uABC <= 0.0f && wCBD <= 0.0f && uBC > 0.0f && vBC > 0.0f)
	{
		A = B;
		B = C;
		float invDenom = 1.0f / (uBC + vBC);
		A.weight = uBC * invDenom;
		B.weight = vBC * invDenom;
		nVerts = 2;
		return;
	}

	// region ca
	if (vABC <= 0.0f && wACD <= 0.0f && uCA > 0.0f && vCA > 0.0f)
	{
		B = A;
		A = C;
		float invDenom = 1.0f / (uCA + vCA);
		A.weight = uCA * invDenom;
		B.weight = vCA * invDenom;
		nVerts = 2;
		return;
	}

	// region dc
	if (vCBD <= 0.0f && uACD <= 0.0f && uDC > 0.0f && vDC > 0.0f)
	{
		A = D;
		B = C;
		float invDenom = 1.0f / (uDC + vDC);
		A.weight = uDC * invDenom;
		B.weight = vDC * invDenom;
		nVerts = 2;
		return;
	}

	// region ad
	if (vACD <= 0.0f && wADB <= 0.0f && uAD > 0.0f && vAD > 0.0f)
	{
		B = D;
		float invDenom = 1.0f / (uAD + vAD);
		A.weight = uAD * invDenom;
		B.weight = vAD * invDenom;
		nVerts = 2;
		return;
	}

	// region bd
	if (uCBD <= 0.0f && uADB <= 0.0f && uBD > 0.0f && vBD > 0.0f)
	{
		A = B;
		B = D;
		float invDenom = 1.0f / (uBD + vBD);
		A.weight = uBD * invDenom;
		B.weight = vBD * invDenom;
		nVerts = 2;
		return;
	}

	// region abc
	if (xABCD <= 0.0f && uABC > 0.0f && vABC > 0.0f && wABC > 0.0f)
	{
		float invDenom = 1.0f / (uABC + vABC + wABC);
		A.weight = uABC * invDenom;
		B.weight = vABC * invDenom;
		C.weight = wABC * invDenom;
		nVerts = 3;
		return;
	}

	// region cbd
	if (uABCD <= 0.0f && uCBD > 0.0f && vCBD > 0.0f && vCBD > 0.0f)
	{
		A = C;
		C = D;
		float invDenom = 1.0f / (uCBD + vCBD + wCBD);
		A.weight = uCBD * invDenom;
		B.weight = vCBD * invDenom;
		C.weight = wCBD * invDenom;
		nVerts = 3;
		return;
	}

	// region acd
	if (vABCD <= 0.0f && uACD > 0.0f && vACD > 0.0f && wACD > 0.0f)
	{
		B = C;
		C = D;
		float invDenom = 1.0f / (uACD + vACD + wACD);
		A.weight = uACD * invDenom;
		B.weight = vACD * invDenom;
		C.weight = wACD * invDenom;
		nVerts = 3;
		return;
	}

	// region adb
	if (wABCD <= 0.0f && uADB > 0.0f && vADB > 0.0F && wADB > 0.0f)
	{
		C = B;
		B = D;
		float invDenom = 1.0f / (uADB + vADB + wADB);
		A.weight = uADB * invDenom;
		B.weight = vADB * invDenom;
		C.weight = wADB * invDenom;
		nVerts = 3;
		return;
	}

	// region abcd
	assert(uABCD > 0.0f && vABCD > 0.0f && wABCD > 0.0f && xABCD > 0.0f);
	A.weight = uABCD;
	B.weight = vABCD;
	C.weight = wABCD;
	D.weight = xABCD;
	nVerts = 4;
}

const glm::vec3 Simplex::FindClosestPoint() const
{
	switch (nVerts)
	{
	case 1:
		return A.support;

	case 2:
		return A.support*A.weight + B.support*B.weight;

	case 3:
		return A.support*A.weight + B.support*B.weight + C.support*C.weight;

	case 4:
		return A.support*A.weight + B.support*B.weight + C.support*C.weight + D.support*D.weight;

	default:
		assert(false);
		return glm::vec3(0.0f);
	}
}

const glm::vec3 Simplex::FindSearchDirection() const
{
	switch (nVerts)
	{
	case 1:
		return -A.support;
	case 2:
	{
		glm::vec3 ba = A.support - B.support;
		return glm::cross(glm::cross(ba, -A.support), ba);
	}
	case 3:
	{
		glm::vec3 n = glm::cross(B.support - A.support, C.support - A.support);

		if (glm::dot(n, A.support) <= 0.0f)
			return n;
		else
			return -n;
	}
	default:
		assert(false);
		return glm::vec3(0.0f);
	}
}

void GJK_Distance(const DistanceInput* input, DistanceOutput* output)
{
	glm::vec3 CA(0.0f);
	glm::vec3 CB(0.0f);
	const DistanceProxy* proxyA = &input->proxyA;
	const DistanceProxy* proxyB = &input->proxyB;
	Transform txA = input->txA;
	Transform txB = input->txB;

	Simplex simplex;
	glm::vec3 dirA = glm::transpose(txA.R) * -glm::vec3(1.0f, 0.0f, 0.0f);
	simplex.A.indexA = proxyA->GetSupport(dirA);
	simplex.A.supportA = txA.R * proxyA->vertices[simplex.A.indexA] + txA.position;
	glm::vec3 dirB = glm::transpose(txB.R) * glm::vec3(1.0f, 0.0f, 0.0f);
	simplex.A.indexB = proxyB->GetSupport(dirB);
	simplex.A.supportB = txB.R * proxyB->vertices[simplex.A.indexB] + txB.position;
	simplex.A.support = simplex.A.supportB - simplex.A.supportA;
	simplex.A.weight = 1.0f;
	simplex.nVerts = 1;

	SimplexVertex* vertices = &simplex.A;

	// Cache the support vertices from previous iteration
	// Terminate if a repeated support point is found on the simplex
	int prevA[4], prevB[4];
	int nVertsPrev = 0;

	float prevDist = FLT_MAX;
	float dist = FLT_MAX;

	int iters = 0;
	while (iters < 20)
	{
		nVertsPrev = simplex.nVerts;
		assert(nVertsPrev <= 4);
		for (int i = 0; i < nVertsPrev; ++i)
		{
			prevA[i] = vertices[i].indexA;
			prevB[i] = vertices[i].indexB;
		}

		// Find new closest point on simplex and remove unused simplex vertices
		switch (simplex.nVerts)
		{
			case 1:
				break;
			case 2:
				simplex.Solve2();
				break;
			case 3:
				simplex.Solve3();
				break;
			case 4:
				simplex.Solve4();
				break;
			default:
				assert(false);
		}

		// tetrahedron contained the origin
		if (simplex.nVerts == 4)
			break;

		glm::vec3 P = simplex.FindClosestPoint();
		dist = glm::length2(P);
		
		// if we moved away from origin, no overlap
		if (dist > prevDist)
			break;

		prevDist = dist;

		glm::vec3 d = simplex.FindSearchDirection();

		if (glm::length2(d) < FLT_EPSILON * FLT_EPSILON)
		{
			// overlap?
			break;
		}

		// Compute a tentative new simplex vertex using support points.
		glm::vec3 dirA = glm::transpose(txA.R) * -d;
		int iA = proxyA->GetSupport(dirA);
		glm::vec3 sA = txA.R * proxyA->vertices[iA] + txA.position;
		glm::vec3 dirB = glm::transpose(txB.R) * d;
		int iB = proxyB->GetSupport(dirB);
		glm::vec3 sB = txB.R * proxyB->vertices[iB] + txB.position;

		++iters;

		// Check for duplicate support points. This is the main termination criteria.
		bool duplicate = false;
		for (int i = 0; i < nVertsPrev; ++i)
		{
			if (iA == prevA[i] && iB == prevB[i])
			{
				duplicate = true;
				break;
			}
		}

		if (duplicate)
		{
			break;
		}

		// Commit new support point to simplex
		SimplexVertex* vertex = vertices + simplex.nVerts;
		vertex->indexA = iA;
		vertex->supportA = sA;
		vertex->indexB = iB;
		vertex->supportB = sB;
		vertex->support = vertex->supportB - vertex->supportA;
		++simplex.nVerts;
	}

	simplex.CalculateClosestPoints(&output->CA, &output->CB);
	output->distance = glm::length(output->CB - output->CA);
}