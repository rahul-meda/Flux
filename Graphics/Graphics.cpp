#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics.h"
#include <iostream>
#include "Texture.h"
#include "../Simulation/Simulation.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../GameObject.h"
#include "../Components/Transform.h"
#include "../Components/Model.h"

Graphics::Graphics()
{
	P = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);
}

Graphics& Graphics::GetInstance()
{
	static Graphics instance;
	return instance;
}

unsigned int Graphics::CreateModel(const ModelDef& modelDef)
{
	unsigned int VAO;
	unsigned int VBO[3];
	unsigned int EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, &VBO[0]);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

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

	models.push_back(Model(VAO, modelDef.indices.size()));

	return models.size() - 1;
}

void Graphics::AddPointLight(glm::vec3 pos)
{
	lightPos.push_back(pos);
}

void Graphics::Update(const std::vector<GameObject>& objects)
{
	glClearColor(0.4f, 0.6f, 0.8f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < Simulation::GetInstance().textureData.textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, Simulation::GetInstance().textureData.textures[i]);
	}

	glm::mat4 T, R, S, VP, MVP;
	VP = P * Simulation::GetInstance().camera.ViewSpace();
	glUseProgram(worldShader);
	unsigned int mvpLoc = glGetUniformLocation(worldShader, "MVP");
	S = glm::mat4(1.0f);

	Transform tx;
	for (int i = 0; i < objects.size(); i++)
	{
		tx = Simulation::GetInstance().transforms[i];
		T = glm::translate(glm::mat4(1.0f), tx.position);
		R = glm::toMat4(tx.orientation);
		MVP = VP * T * R;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		Model m = models[objects[i].modelID];	//cache friendly? probably need a better way to retrieve model data
		glBindVertexArray(m.VAO);
		glDrawElements(GL_TRIANGLES, m.nIndices, GL_UNSIGNED_INT, 0);
	}
	glUseProgram(0);

	glUseProgram(lightShader);
	mvpLoc = glGetUniformLocation(lightShader, "MVP");
	for (int i = 0; i < lightPos.size(); i++)
	{
		T = glm::translate(glm::mat4(1.0f), lightPos[i]);
		S = glm::scale(S, glm::vec3(0.3f));
		MVP = VP * T * S;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

		Model m = models[lightModelID];
		glBindVertexArray(m.VAO);
		glDrawElements(GL_TRIANGLES, m.nIndices, GL_UNSIGNED_INT, 0);
	}
	glUseProgram(0);
}
