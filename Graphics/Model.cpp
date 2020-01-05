#include "Model.h"
#include "glad/glad.h"
#include "../Simulation/Simulation.h"

Model::Model(const ModelDef& modelDef)
	: m_nIndices(modelDef.indices.size()),
	  m_texture(modelDef.textureID)
{
	unsigned int VBO[3];
	unsigned int EBO;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(3, &VBO[0]);
	glGenBuffers(1, &EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(modelDef.vertices), 0, GL_STATIC_DRAW);
	GLfloat* vBuffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	glm::vec3* verts = (glm::vec3*)(vBuffer);
	for each (auto v in modelDef.vertices)
	{
		*verts++ = v;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(modelDef.colors), 0, GL_STATIC_DRAW);
	GLfloat* cBuffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	glm::vec3* cols = (glm::vec3*)(cBuffer);
	for each (auto c in modelDef.colors)
	{
		*cols++ = c;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(modelDef.textureCoords), 0, GL_STATIC_DRAW);
	GLfloat* tBuffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	glm::vec2* texCoords = (glm::vec2*)(tBuffer);
	for each (auto t in modelDef.textureCoords)
	{
		*texCoords++ = t;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(modelDef.indices), 0, GL_STATIC_DRAW);
	GLuint* iBuffer = static_cast<GLuint*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
	GLuint* inds = iBuffer;
	for each (auto i in modelDef.indices)
	{
		*inds++ = i;
	}
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

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
