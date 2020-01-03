#include "Model.h"
#include "glad/glad.h"
#include "../Simulation/Simulation.h"

Model::Model(std::vector<glm::vec3>& vertices, std::vector<int>& indices)
	: m_nIndices(indices.size())
{
	unsigned int VBO, EBO;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), 0, GL_STATIC_DRAW);
	GLfloat* pBuffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	glm::vec3* verts = (glm::vec3*)(pBuffer);
	for each (auto v in vertices)
	{
		*verts++ = v;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), 0, GL_STATIC_DRAW);
	GLuint* pIBuffer = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	GLuint* inds = pIBuffer;
	for each (auto i in indices)
	{
		*inds++ = i;
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Model::~Model()
{
	/*auto bufferIDs = (Simulation::GetInstance().bufferIDmap.find(m_VAO))->second;

	unsigned int VBO = bufferIDs.first;
	unsigned int EBO = bufferIDs.second;

	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	Simulation::GetInstance().bufferIDmap.erase(m_VAO);*/
}
