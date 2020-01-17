#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics.h"
#include <iostream>
#include "Texture.h"
#include "../Simulation/Simulation.h"
#include "../Physics/Physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../GameObject.h"
#include "../Components/Transform.h"
#include "../Components/Model.h"

Graphics::Graphics()
{}

Graphics& Graphics::GetInstance()
{
	static Graphics instance;
	return instance;
}

unsigned int Graphics::CreateModel(const ModelDef& modelDef)
{
	unsigned int VAO;
	unsigned int VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	std::vector<glm::vec3> vData;
	for(int i = 0; i < modelDef.vertices.size(); i++)
	{
		vData.push_back(modelDef.vertices[i]);
		vData.push_back(modelDef.normals[i]);
	}

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vData.size() * 3 * sizeof(float), 0, GL_STATIC_DRAW);
	GLfloat* vBuffer = static_cast<GLfloat*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	glm::vec3* verts = (glm::vec3*)(vBuffer);
	for each (auto v in vData)
	{
		*verts++ = v;
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	models.push_back(Model(VAO, modelDef.vertices.size()));

	return models.size() - 1;
}

void Graphics::AddPointLight(glm::vec3 pos)
{
	lightPos.push_back(pos);

	glUseProgram(worldShader);
	glUniform3fv(glGetUniformLocation(worldShader, "lightPos"), 1, glm::value_ptr(lightPos[0]));
	glUseProgram(0);
}

void Graphics::Update(const std::vector<GameObject>& objects)
{
	//glClearColor(0.4f, 0.6f, 0.8f, 1.0f);
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < Simulation::GetInstance().textureData.textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Simulation::GetInstance().textureData.textures[i]);
	}

	glm::mat4 T, R, S, M, VP, MVP;
	VP = P * Simulation::GetInstance().camera.ViewSpace();
	glUseProgram(worldShader);
	unsigned int mvpLoc = glGetUniformLocation(worldShader, "MVP");
	unsigned int mLoc = glGetUniformLocation(worldShader, "M");
	unsigned int eyeLoc = glGetUniformLocation(worldShader, "eyePos");
	glUniform3fv(eyeLoc, 1, glm::value_ptr(Simulation::GetInstance().camera.position));

	auto N = objects.size();
	for (int i = 0; i < N; i++)
	{
		//tx.orientation = glm::normalize(glm::angleAxis((float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f)));
		T = glm::translate(glm::mat4(1.0), Physics::GetInstance().positions[i].p);
		R = glm::toMat4(Physics::GetInstance().positions[i].q);
		S = glm::scale(scales[i]);
		M = T * R * S;
		MVP = VP * M;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));

		Model m = models[objects[i].modelID];	//cache friendly? probably need a better way to retrieve model data
		glBindVertexArray(m.VAO);
		//glDrawElements(GL_TRIANGLES, m.nIndices, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
	}
	glUseProgram(0);

	glUseProgram(lightShader);
	mvpLoc = glGetUniformLocation(lightShader, "MVP");
	//lightPos[0].x = 10.0f * cos(glfwGetTime());
	//lightPos[0].z = 10.0f * sin(glfwGetTime());
	for (int i = 0; i < lightPos.size(); i++)
	{
		T = glm::translate(glm::mat4(1.0f), lightPos[i]);
		S = glm::scale(glm::vec3(0.3f));
		MVP = VP * T * S;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUseProgram(0);

		glUseProgram(worldShader);
		glUniform3fv(glGetUniformLocation(worldShader, "lightPos"), 1, glm::value_ptr(lightPos[0]));
		glUseProgram(0);

		glUseProgram(lightShader);
		Model m = models[lightModelID];
		glBindVertexArray(m.VAO);
		//glDrawElements(GL_TRIANGLES, m.nIndices, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
		glUseProgram(lightShader);
	}
}
