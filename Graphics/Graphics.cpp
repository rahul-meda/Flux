#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics.h"
#include "Model.h"
#include <iostream>
#include "Texture.h"
#include "../Simulation/Simulation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < Simulation::GetInstance().textureData.textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Simulation::GetInstance().textureData.textures[i]);
	}

	glUseProgram(shaderProgram);

	glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.5f));
	glm::quat q = glm::angleAxis(glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 R = glm::toMat4(q);
	glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
	glm::mat4 M;
	glm::vec3 pos = glm::vec3(10.0f * sin(glfwGetTime()), 0.0f,  10.0f * cos(glfwGetTime()));
	glm::mat4 V = Simulation::GetInstance().camera.ViewSpace();
	glm::mat4 P = glm::perspective(glm::radians(45.0f), 1024.0f/768.0f, 0.1f, 100.0f);
	glm::mat4 MVP;

	q = glm::angleAxis(-(float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
	R = glm::toMat4(q);
	M = R;
	MVP = P * V;
	unsigned int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

	for each (auto& model in models)
	{
		glBindVertexArray(model.m_VAO);
		glDrawElements(GL_TRIANGLES, model.m_nIndices, GL_UNSIGNED_INT, 0);
		//glDrawArrays(GL_TRIANGLES, 0, 36);
		glUseProgram(0);
	}
}
