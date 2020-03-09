
inline bool Overlap(AABB* A, AABB* B)
{
	if (A->max.x < B->min.x || A->min.x > B->max.x)
		return false;
	if (A->max.y < B->min.y || A->min.y > B->max.y)
		return false;
	if (A->max.z < B->min.z || A->min.z > B->max.z)
		return false;

	return true;
}

inline void ComputeBasis(const glm::vec3& v1, glm::vec3* v2, glm::vec3* v3)
{
	// At least one component of a unit vector must be greater than 0.57735

	if (glm::abs(v1.x) >= 0.57735)
		*v2 = glm::vec3(v1.y, -v1.x, 0.0f);
	else
		*v2 = glm::vec3(0.0f, v1.z, -v1.y);

	*v2 = glm::normalize(*v2);

	*v3 = glm::normalize(glm::cross(v1, *v2));
}

inline const glm::vec3 FxIntersect(const glm::vec3& a, const glm::vec3& b, float da, float db)
{
	return a + (b - a) * (da / (da - db));
}

inline bool FxParallel(const glm::vec3& a, const glm::vec3& b, float kTol)
{
	float k = glm::length(a) / glm::length(b);
	glm::vec3 bk = b * k;

	if (abs(a.x - bk.x) < kTol &&
		abs(a.y - bk.y) < kTol &&
		abs(a.z - bk.z) < kTol)
	{
		return true;
	}

	return false;
}