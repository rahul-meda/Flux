#pragma once

#include <vector>
#include <glm/vec3.hpp>

class Model
{
public:
	Model(std::vector<glm::vec3>& vertices, std::vector<int>& indices);

	~Model();

	unsigned int m_VAO;

	unsigned int m_nIndices;
};