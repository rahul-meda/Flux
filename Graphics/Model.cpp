#include "Model.h"
#include "glad/glad.h"
#include "../Simulation/Simulation.h"
#include "Graphics.h"

Model::Model(unsigned int vaoID, unsigned int nIndices)
	: VAO(vaoID), nIndices(nIndices)
{}

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
