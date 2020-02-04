#pragma once

#include "../Components/Model.h"
#include <vector>

#define PI 3.14159265359f

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
		md.vertices.push_back(vertices[indices[i]]);
		md.normals.push_back(vertices[indices[i]]);
	}
}