
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

inline bool HalfSpace::Infront(const glm::vec3& point) const
{
	return Distance(point) > 0.0f;
}

inline bool HalfSpace::Behind(const glm::vec3& point) const
{
	return Distance(point) < 0.0f;
}

inline bool HalfSpace::On(const glm::vec3& point) const
{
	float dist = Distance(point);

	return (dist < EPSILON && dist > -EPSILON);
}

void CreateLine(ModelDef& md)
{
	md.vertices.push_back(glm::vec3(0.0f));
	md.vertices.push_back(glm::vec3(0.5f, 0.0f, 0.0f));
}

#define slices 36
#define stacks 12

void TriangulateSphere(ModelDef& to, ModelDef& from)
{
	int k1, k2;
	for (int i = 0; i < stacks; ++i)
	{
		k1 = i * (slices + 1);     // beginning of current stack
		k2 = k1 + slices + 1;      // beginning of next stack

		for (int j = 0; j < slices; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0)
			{
				to.vertices.push_back(from.vertices[k1]);
				to.vertices.push_back(from.vertices[k2]);
				to.vertices.push_back(from.vertices[k1 + 1]);
				to.normals.push_back(from.normals[k1]);
				to.normals.push_back(from.normals[k2]);
				to.normals.push_back(from.normals[k1 + 1]);
				to.textureCoords.push_back(from.textureCoords[k1]);
				to.textureCoords.push_back(from.textureCoords[k2]);
				to.textureCoords.push_back(from.textureCoords[k1 + 1]);
			}

			// k1+1 => k2 => k2+1
			if (i != (stacks - 1))
			{
				to.vertices.push_back(from.vertices[k1 + 1]);
				to.vertices.push_back(from.vertices[k2]);
				to.vertices.push_back(from.vertices[k2 + 1]);
				to.normals.push_back(from.normals[k1 + 1]);
				to.normals.push_back(from.normals[k2]);
				to.normals.push_back(from.normals[k2 + 1]);
				to.textureCoords.push_back(from.textureCoords[k1 + 1]);
				to.textureCoords.push_back(from.textureCoords[k2]);
				to.textureCoords.push_back(from.textureCoords[k2 + 1]);
			}
		}
	}
}

void CreateSphere(ModelDef& md)
{
	float sectorStep = 2.0f * PI / slices;
	float stackStep = PI / stacks;
	float sectorAngle, stackAngle;
	ModelDef tmp;

	for (int i = 0; i <= stacks; ++i)
	{
		stackAngle = PI / 2.0f - (float)i * stackStep;      // starting from pi/2 to -pi/2
		float xz = cosf(stackAngle);						// r * cos(u)
		float y = sinf(stackAngle);							// r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= slices; ++j)
		{
			sectorAngle = (float)j * sectorStep;    // starting from 0 to 2pi

			// vertex position (x, y, z)
			float x = xz * sinf(sectorAngle);             // r * cos(u) * sin(v)
			float z = xz * cosf(sectorAngle);             // r * cos(u) * cos(v)
			tmp.vertices.push_back(glm::vec3(x, y, z));

			// vertex normal (nx, ny, nz)
			tmp.normals.push_back(glm::vec3(x, y, z));

			// vertex tex coord (s, t) range between [0, 1]
			float u = (float)j / slices;
			float v = (float)i / stacks;
			tmp.textureCoords.push_back(glm::vec3(u, v, 0.0f));
		}
	}

	TriangulateSphere(md, tmp);
}

void CreateCapsule(ModelDef& md)
{
	CreateSphere(md);
	std::vector<int> indices1;
	std::vector<int> indices2;

	int N = md.vertices.size();
	for (int i = 0; i < N/2; ++i)
	{
		if ((md.vertices[i].y) == 0.0f)
		{
			indices1.push_back(i);
		}

		md.vertices[i].y += 1.0f;
	}
	
	for (int i = N / 2; i < N; ++i)
	{
		if ((md.vertices[i].y) == 0.0f)
		{
			indices2.push_back(i);
		}

		md.vertices[i].y -= 1.0f;
	}

	for (int i = 0; i < 3 * slices - 1; ++i)
	{
		int i1 = indices1[i];
		int i2 = indices1[i + 1];
		int i3 = indices2[i];
		int i4 = indices2[i + 1];

		glm::vec3 v1 = md.vertices[i1];
		glm::vec3 v2 = md.vertices[i2];
		glm::vec3 v3 = md.vertices[i3];
		glm::vec3 v4 = md.vertices[i4];

		glm::vec3 n = glm::normalize(glm::cross(v1 - v2, v3 - v1));

		md.vertices.push_back(v1);
		md.vertices.push_back(v3);
		md.vertices.push_back(v2);
		md.vertices.push_back(v2);
		md.vertices.push_back(v3);
		md.vertices.push_back(v4);
		md.normals.push_back(md.normals[i1]);
		md.normals.push_back(md.normals[i3]);
		md.normals.push_back(md.normals[i2]);
		md.normals.push_back(md.normals[i2]);
		md.normals.push_back(md.normals[i3]);
		md.normals.push_back(md.normals[i4]);
		md.textureCoords.push_back(v1);
		md.textureCoords.push_back(v3);
		md.textureCoords.push_back(v2);
		md.textureCoords.push_back(v2);
		md.textureCoords.push_back(v3);
		md.textureCoords.push_back(v4);
	}
}

#define sectors 32

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

		glm::vec3 n = -glm::normalize(glm::cross(v2 - v1, v3 - v1));

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

	glm::vec3 n = -glm::normalize(glm::cross(v2 - v1, v3 - v1));

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