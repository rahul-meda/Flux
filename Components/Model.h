#pragma once

#include <vector>
#include <glm/glm.hpp>

struct ModelDef
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec2> textureCoords;
	std::vector<unsigned int> indices;
};

class Model
{
public:
	Model(unsigned int vaoID, unsigned int nIndices);

	~Model();

	unsigned int VAO;

	unsigned int nIndices;
};
