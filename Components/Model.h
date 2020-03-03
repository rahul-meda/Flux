#pragma once

#include <vector>
#include <glm/glm.hpp>

struct ModelDef
{
	std::vector<glm::vec3> vertices;
	std::vector<unsigned int> indices;
	std::vector<glm::vec3> normals;	// vertex normals
	std::vector<glm::vec3> textureCoords;
};

class Model
{
public:
	Model(unsigned int vaoID, unsigned int nIndices);

	~Model();

	unsigned int VAO;

	unsigned int nIndices;
};
