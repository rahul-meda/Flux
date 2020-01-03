#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics.h"
#include "Model.h"
#include <iostream>

Graphics::Graphics()
{}

Graphics& Graphics::GetInstance()
{
	static Graphics instance;
	return instance;
}

void Graphics::Update(int shaderProgram)
{
	for each (auto& model in models)
	{
		glClearColor(0.4f, 0.6f, 0.8f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(model.m_VAO);
		glDrawElements(GL_TRIANGLES, model.m_nIndices, GL_UNSIGNED_INT, 0);
		glUseProgram(0);
	}
}
