#pragma once

#include <vector>
#include <glm/glm.hpp>

struct ModelDef
{
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	std::vector<glm::vec2> textureCoords;
	std::vector<unsigned int> indices;
	unsigned int textureID;
};

class Model
{
public:
	Model(const ModelDef& modelDef);

	~Model();

	unsigned int m_VAO;

	unsigned int m_nIndices;

	unsigned int m_texture;
};
