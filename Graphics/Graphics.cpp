
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

void Graphics::Initialize()
{
	glUseProgram(worldShader);
	glUniform3f(glGetUniformLocation(worldShader, "lightColor"), 1.0f, 1.0f, 1.0f);
	glUseProgram(0);
}

unsigned int Graphics::CreateModel(const ModelDef& modelDef)
{
	unsigned int VAO;
	unsigned int VBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	std::vector<glm::vec3> vData;
	int n = modelDef.vertices.size();
	for(int i = 0; i < n; ++i)
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
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < Simulation::GetInstance().textureData.textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Simulation::GetInstance().textureData.textures[i]);
	}

	glm::mat4 T, R, S, M, VP, MVP;
	VP = P * Simulation::GetInstance().camera.ViewSpace();

	if (Physics::GetInstance().debugDraw)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(worldShader);
	unsigned int mvpLoc = glGetUniformLocation(worldShader, "MVP");
	unsigned int mLoc = glGetUniformLocation(worldShader, "M");
	unsigned int eyeLoc = glGetUniformLocation(worldShader, "eyePos");
	unsigned int colorLoc = glGetUniformLocation(worldShader, "objColor");
	glm::vec3 eye = Simulation::GetInstance().camera.position;
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));

	int N = objects.size();
	for (int i = 0; i < N; i++)
	{
		GameObject obj = objects[i];

		T = glm::translate(glm::mat4(1.0f), Physics::GetInstance().bodies[i]->GetPosition());
		R = glm::toMat4(Physics::GetInstance().positions[i].q);
		S = glm::scale(scales[i]);
		M = T * R * S;
		MVP = VP * M;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));
		glUniform3fv(colorLoc, 1, glm::value_ptr(obj.color));

		Model m = models[obj.modelID];	//cache friendly? ToDo: try storing pointers to model in GO
		glBindVertexArray(m.VAO);
		glLineWidth(2.0f);
		//glDrawElements(GL_TRIANGLES, m.nIndices, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
	}

	N = points.size();
	for (int i = 0; i < N; ++i)
	{
		R_Point p = points[i];
		T = glm::translate(glm::mat4(1.0f), p.pos);
		S = glm::scale(glm::vec3(0.1f));
		M = T * S;
		MVP = VP * M;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));
		glUniform3fv(colorLoc, 1, glm::value_ptr(p.color));
		Model m = models[cubeModelID];
		glBindVertexArray(m.VAO);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
	}

	N = lines.size();
	for (int i = 0; i < N; ++i)
	{
		R_Line l = lines[i];
		T = glm::translate(glm::mat4(1.0f), l.pos);
		R = glm::toMat4(l.rot);
		M = T * R;
		MVP = VP * M;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));
		glUniform3fv(colorLoc, 1, glm::value_ptr(l.color));
		Model m = models[lineModelID];
		glBindVertexArray(m.VAO);
		glLineWidth(5.0f);
		glDrawArrays(GL_LINES, 0, m.nIndices);
	}
	glUseProgram(0);

	glUseProgram(lightShader);
	mvpLoc = glGetUniformLocation(lightShader, "MVP");
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
		Model m = models[cubeModelID];
		glBindVertexArray(m.VAO);
		//glDrawElements(GL_TRIANGLES, m.nIndices, GL_UNSIGNED_INT, 0);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
		glUseProgram(lightShader);
	}
}
