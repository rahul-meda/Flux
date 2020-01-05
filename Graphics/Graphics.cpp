#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics.h"
#include "Model.h"
#include <iostream>
#include "Texture.h"
#include "../Simulation/Simulation.h"

Graphics::Graphics()
{}

Graphics& Graphics::GetInstance()
{
	static Graphics instance;
	return instance;
}

void Graphics::Update(int shaderProgram)
{
	glClearColor(0.4f, 0.6f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < Simulation::GetInstance().textureData.textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Simulation::GetInstance().textureData.textures[i]);
	}

	glUseProgram(shaderProgram);

	for each (auto& model in models)
	{
		glBindVertexArray(model.m_VAO);
		glDrawElements(GL_TRIANGLES, model.m_nIndices, GL_UNSIGNED_INT, 0);
		glUseProgram(0);
	}
}
