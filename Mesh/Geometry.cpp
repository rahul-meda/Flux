
#include "Geometry.h"

#define EPSILON 0.005f

HalfSpace::HalfSpace()
{}

HalfSpace::HalfSpace(const glm::vec3& normal, float distance)
	: normal(normal), distance(distance)
{}

HalfSpace::HalfSpace(const glm::vec3& normal, const glm::vec3& point)
	: normal(normal), distance(glm::dot(point, normal))
{}

glm::vec3 HalfSpace::Origin() const
{
	return distance * normal;
}

float HalfSpace::Distance(const glm::vec3& point) const
{
	return glm::dot(point, normal) - distance;
}

glm::vec3 HalfSpace::Projection(const glm::vec3& point) const
{
	return point - Distance(point) * normal;
}

bool HalfSpace::Infront(const glm::vec3& point) const
{
	return Distance(point) > 0.0f;
}

bool HalfSpace::Behind(const glm::vec3& point) const
{
	return Distance(point) < 0.0f;
}

bool HalfSpace::On(const glm::vec3& point) const
{
	float dist = Distance(point);

	return (dist < EPSILON && dist > -EPSILON);
}

void CreateLine(ModelDef& md)
{
	md.vertices.push_back(glm::vec3(0.0f));
	md.vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
}

void CreateSphere(ModelDef& md)
{
	const int slices = 20;
	const int stacks = 20;
	float R = 1.0f / (float)(slices - 1);
	float S = 1.0f / (float)(stacks - 1);
	std::vector<glm::vec3> vertices;
	std::vector<int> indices;

	for (int r = 0; r < slices; ++r)
	{
		for (int s = 0; s < stacks; ++s)
		{
			float y = (float)(sin(-PI*0.5f + PI * r * R));
			float x = (float)(cos(2 * PI * s * S) * sin(PI * r * R));
			float z = (float)(sin(2 * PI * s * S) * sin(PI * r * R));

			vertices.push_back(glm::vec3(x, y, z));

			int curRow = r * stacks;
			int nextRow = (r + 1) * stacks;

			indices.push_back(curRow + s);
			indices.push_back(nextRow + s);
			indices.push_back(nextRow + (s + 1));

			indices.push_back(curRow + s);
			indices.push_back(nextRow + (s + 1));
			indices.push_back(curRow + (s + 1));
		}
	}

	int N = indices.size();
	for (int i = 0; i < N; ++i)
	{
		if (indices[i] > slices * stacks - 1) continue;

		glm::vec3 v = vertices[indices[i]];
		md.vertices.push_back(v);
		md.normals.push_back(v);

		float tu = (asinf(v.x) / PI) + 0.5f;
		float tv = (asinf(v.y) / PI) + 0.5f;
		md.textureCoords.push_back(glm::vec3(tu, tv, 0.0f));
	}
}

#define sectors 20

void CreateCircle(std::vector<glm::vec3>& vertices)
{
	float sectorStep = 2.0f * PI / sectors;
	float sectorAngle;  // radians

	for (int i = 0; i < sectors; ++i)
	{
		sectorAngle = (float)i * sectorStep;
		vertices.push_back(glm::vec3(cosf(sectorAngle), 0.0f, sinf(sectorAngle)));
	}
}

void RecalculateNormals(ModelDef& md)
{
	md.normals.clear();

	int N = md.vertices.size();
	for (int i = 0; i <= N - 3; i += 3)
	{
		glm::vec3 v1 = md.vertices[i];
		glm::vec3 v2 = md.vertices[i + 1];
		glm::vec3 v3 = md.vertices[i + 2];
		glm::vec3 n = glm::normalize(glm::cross(v2 - v1, v3 - v1));

		for (int j = 0; j < 3; ++j)
		{
			md.normals.push_back(n);
		}
	}
}

void TriangulateCylinder(ModelDef& to, ModelDef& from)
{
	// generate CCW list of cylinder triangles
	int k1 = 0;                 // 1st vertex index at base
	int k2 = sectors;           // 1st vertex index at top

	for (int i = 0; i < sectors - 1; ++i, ++k1, ++k2)
	{
		glm::vec3 v1 = from.vertices[k1];
		glm::vec3 v2 = from.vertices[k1 + 1];
		glm::vec3 v3 = from.vertices[k2];

		to.vertices.push_back(v1);
		to.textureCoords.push_back(glm::vec3((float)i / sectors, 0.0f, 0.0f));
		to.vertices.push_back(v2);
		to.textureCoords.push_back(glm::vec3((i + 1.0f) / sectors, 0.0f, 0.0f));
		to.vertices.push_back(v3);
		to.textureCoords.push_back(glm::vec3((float)i / sectors, 1.0f, 0.0f));
		to.vertices.push_back(v3);
		to.textureCoords.push_back(glm::vec3((float)i / sectors, 1.0f, 0.0f));
		to.vertices.push_back(v2);
		to.textureCoords.push_back(glm::vec3((i + 1.0f) / sectors, 0.0f, 0.0f));
		to.vertices.push_back(from.vertices[k2 + 1]);
		to.textureCoords.push_back(glm::vec3((i + 1.0f) / sectors, 1.0f, 0.0f));

		glm::vec3 n = glm::normalize(glm::cross(v2 - v1, v3 - v1));

		for (int j = 0; j < 6; ++j)
			to.normals.push_back(n);
	}

	glm::vec3 v1 = from.vertices[sectors - 1];
	glm::vec3 v2 = from.vertices[0];
	glm::vec3 v3 = from.vertices[2 * sectors - 1];
	float vt = (sectors - 1.0f) / (float)sectors;

	to.vertices.push_back(v1);
	to.textureCoords.push_back(glm::vec3(vt, 0.0f, 0.0f));
	to.vertices.push_back(v2);
	to.textureCoords.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	to.vertices.push_back(v3);
	to.textureCoords.push_back(glm::vec3(vt, 1.0f, 0.0f));
	to.vertices.push_back(v3);
	to.textureCoords.push_back(glm::vec3(vt, 1.0f, 0.0f));
	to.vertices.push_back(v2);
	to.textureCoords.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	to.vertices.push_back(from.vertices[sectors]);
	to.textureCoords.push_back(glm::vec3(1.0f, 1.0f, 0.0f));

	glm::vec3 n = glm::normalize(glm::cross(v2 - v1, v3 - v1));

	for (int j = 0; j < 6; ++j)
		to.normals.push_back(n);

	glm::vec3 v0 = glm::vec3(0.0f, -1.0f, 0.0f);
	float step = 2.0f * PI / (float)sectors;

	for (int i = 0; i < sectors - 1; ++i)
	{
		float angle1 = (float)i * step;
		float angle2 = (i + 1.0f) * step;

		to.vertices.push_back(v0);
		to.textureCoords.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
		to.vertices.push_back(from.vertices[i + 1]);
		to.textureCoords.push_back(glm::vec3(0.5f * cos(angle2) + 0.5f, 0.5f * sin(angle2) + 0.5f, 0.0f));
		to.vertices.push_back(from.vertices[i]);
		to.textureCoords.push_back(glm::vec3(0.5f * cos(angle1) + 0.5f, 0.5f * sin(angle1) + 0.5f, 0.0f));

		for (int j = 0; j < 3; ++j)
			to.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	}

	float angle = ((sectors - 1.0f) / (float)sectors) * 2.0f * PI;

	to.vertices.push_back(v0);
	to.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	to.textureCoords.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
	to.vertices.push_back(from.vertices[0]);
	to.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	to.textureCoords.push_back(glm::vec3(1.0f, 0.5f, 0.0f));
	to.vertices.push_back(from.vertices[sectors - 1]);
	to.normals.push_back(glm::vec3(0.0f, -1.0f, 0.0f));
	to.textureCoords.push_back(glm::vec3(0.5f * cos(angle) + 0.5f, 0.5f * sinf(angle) + 0.5f, 0.0f));	

	v0 = glm::vec3(0.0f, 1.0f, 0.0f);
	int k = 0;

	for (int i = sectors; k < sectors - 1; ++i, ++k)
	{
		float angle1 = (float)i * step;
		float angle2 = (i + 1.0f) * step;

		to.vertices.push_back(v0);
		to.textureCoords.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
		to.vertices.push_back(from.vertices[i]);
		to.textureCoords.push_back(glm::vec3(0.5f * cos(angle1) + 0.5f, 0.5f * sin(angle1) + 0.5f, 0.0f));
		to.vertices.push_back(from.vertices[i + 1]);
		to.textureCoords.push_back(glm::vec3(0.5f * cos(angle2) + 0.5f, 0.5f * sin(angle2) + 0.5f, 0.0f));

		for (int j = 0; j < 3; ++j)
			to.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	}

	to.vertices.push_back(v0);
	to.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	to.textureCoords.push_back(glm::vec3(0.5f, 0.5f, 0.0f));
	to.vertices.push_back(from.vertices[sectors]);
	to.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	to.textureCoords.push_back(glm::vec3(0.5f * cos(angle) + 0.5f, 0.5f * sinf(angle) + 0.5f, 0.0f));
	to.vertices.push_back(from.vertices[2 * sectors - 1]);
	to.normals.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	to.textureCoords.push_back(glm::vec3(1.0f, 0.5f, 0.0f));
}

void CreateCylinder(ModelDef& md)
{
	// get unit circle vectors on XZ-plane
	std::vector<glm::vec3> unitVertices;
	CreateCircle(unitVertices);

	ModelDef tmp;

	// put side vertices to arrays
	for (int i = 0; i < 2; ++i)
	{
		float h = -1.0f + (float)i * 2.0f;           

		for (int j = 0; j < sectors; ++j)
		{
			glm::vec3 v = unitVertices[j];

			tmp.vertices.push_back(glm::vec3(v.x, h, v.z));
		}
	}

	TriangulateCylinder(md, tmp);
}