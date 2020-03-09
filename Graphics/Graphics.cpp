
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "Graphics.h"
#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"
#include <iostream>
#include <string>
#include "../Simulation/Simulation.h"
#include "../Physics/Physics.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "../Components/Transform.h"
#include "../Mesh/Geometry.h"
#include "../Mesh/ObjParser.h"

Graphics::Graphics()
{}

Graphics& Graphics::GetInstance()
{
	static Graphics instance;
	return instance;
}

void Graphics::Initialize()
{
	// draw the pixel only if the object is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enables depth-testing
	glDepthFunc(GL_LESS);    // interpret smaller values as closer

	ModelDef box, line, sphere, cylinder, capsule;
	HMesh mesh;

	ParseObj("Resources/Models/Box.obj", mesh);
	mesh.GetModelData(box);
	cubeModelID = CreateModel(box);

	CreateLine(line);
	lineModelID = CreateModel(line);

	CreateSphere(sphere);
	sphereModelID = CreateModel(sphere);

	CreateCylinder(cylinder);
	cylinderModelID = CreateModel(cylinder);

	CreateCapsule(capsule);
	capsuleModelID = CreateModel(capsule);

	textureLocs[0] = "diffuseTexture";
	textureLocs[1] = "specularTexture";
	textureLocs[2] = "emissionTexture";

	unsigned int hingeTexture = CreateTexture("resources/textures/metal2.jpg");

	hingeMaterial.diffuseMap = hingeTexture;
	hingeMaterial.specularMap = hingeTexture;
	hingeMaterial.count = 2;

	lightColors[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	lightColors[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	lightColors[2] = glm::vec3(0.0f, 0.0f, 1.0f);
	lightColors[3] = glm::vec3(0.7f, 0.1f, 1.0f);
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
	unsigned int stride = 1;
	bool vn = modelDef.normals.size() > 0 ? true : false;

	if (vn)
	{
		stride = 3;
		for (int i = 0; i < n; ++i)
		{
			vData.push_back(modelDef.vertices[i]);
			vData.push_back(modelDef.normals[i]);
			vData.push_back(modelDef.textureCoords[i]);
		}
	}
	else
	{
		for (int i = 0; i < n; ++i)
		{
			vData.push_back(modelDef.vertices[i]);
		}
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

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * stride * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	if (vn)
	{
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * stride * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * stride * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	Model m;
	m.VAO = VAO;
	m.nIndices = n;
	models.push_back(m);

	return models.size() - 1;
}

unsigned int Graphics::CreateTexture(const char* filePath)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(false);

	unsigned char *data = stbi_load(filePath, &width, &height, &nrChannels, 0);
	if (!data)
	{
		std::cout << "Failed to load texture" << std::endl;
		return 0;
	}
	GLenum format = GL_RGB;
	if (nrChannels == 1)
		format = GL_RED;
	else if (nrChannels == 3)
		format = GL_RGB;
	else if (nrChannels == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	return texture;
}

void Graphics::AddPointLight(glm::vec3 pos)
{
	lightPos.push_back(pos);
	int i = lightPos.size() - 1;
	std::string loc = "lightPos[" + std::to_string(i) + "]";

	glUseProgram(worldShader);
	glUniform3fv(glGetUniformLocation(worldShader, loc.c_str()), 1, glm::value_ptr(lightPos[i]));
	glUseProgram(0);
}

void Graphics::Update()
{
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 T, R, S, M, VP, MVP;
	VP = P * Simulation::GetInstance().camera.ViewSpace();

	if (Physics::GetInstance().debugDraw)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 0);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(worldShader);
	unsigned int mvpLoc = glGetUniformLocation(worldShader, "MVP");
	unsigned int mLoc = glGetUniformLocation(worldShader, "M");
	unsigned int eyeLoc = glGetUniformLocation(worldShader, "eyePos");
	unsigned int lightMapLoc = glGetUniformLocation(worldShader, "lightMap");
	glm::vec3 eye = Simulation::GetInstance().camera.position;
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));
	glUniform3fv(glGetUniformLocation(worldShader, "lightPos[4]"), 1, glm::value_ptr(eye));
	glUniform1f(glGetUniformLocation(worldShader, "time"), glfwGetTime());

	int N = objects.size();
	for (int i = 0; i < N; i++)
	{
		R_Object obj = objects[i];

		int nM = obj.modelIDs.size();
		if (nM == 3)
		{
			int x = 1;
		}
		for (int j = 0; j < nM; ++j)
		{
			T = glm::translate(glm::mat4(1.0f), obj.pos + obj.rot * obj.posOffsets[j]);
			R = glm::mat4(obj.rot * obj.rotOffsets[j]);
			S = glm::scale(obj.scales[j]);
			M = T * R * S;
			MVP = VP * M;

			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));

			Model m = models[obj.modelIDs[j]];	
			Material material = obj.materials[j];
			unsigned int* mapID = &material.diffuseMap;
			int C = material.count;
			for (unsigned int c = 0; c < C; ++c)
			{
				unsigned int tID = *(mapID + c);
				glActiveTexture(GL_TEXTURE0 + tID - 1);
				glBindTexture(GL_TEXTURE_2D, tID);

				glUniform1i(glGetUniformLocation(worldShader, textureLocs[c]), tID - 1);
			}

			glm::vec3 lightMap(1.0f);
			if (C == 1)
				lightMap.y = lightMap.z = 0.0f;
			else if (C == 2)
				lightMap.z = 0.0f;

			glUniform3fv(lightMapLoc, 1, glm::value_ptr(lightMap));
			glBindVertexArray(m.VAO);
			glLineWidth(2.0f);
			glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
		}
	}

	N = hinges.size();
	for (int i = 0; i < N; ++i)
	{
		R_Hinge h = hinges[i];

		T = glm::translate(glm::mat4(1.0f), h.pos);
		R = glm::mat4(h.rot);
		S = glm::scale(glm::vec3(0.25f, h.scale, 0.25f));
		M = T * R * S;
		MVP = VP * M;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));

		Model m = models[cylinderModelID];	//cache friendly? ToDo: try storing pointers to model in GO
		unsigned int* mapID = &hingeMaterial.diffuseMap;
		int C = hingeMaterial.count;
		for (unsigned int c = 0; c < C; ++c)
		{
			unsigned int tID = *(mapID + c);
			glActiveTexture(GL_TEXTURE0 + tID - 1);
			glBindTexture(GL_TEXTURE_2D, tID);

			glUniform1i(glGetUniformLocation(worldShader, textureLocs[c]), tID - 1);
		}

		glm::vec3 lightMap(1.0f);
		if (C == 1)
			lightMap.y = lightMap.z = 0.0f;
		else if (C == 2)
			lightMap.z = 0.0f;

		glUniform3fv(lightMapLoc, 1, glm::value_ptr(lightMap));
		glBindVertexArray(m.VAO);
		glLineWidth(2.0f);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
	}
	glUseProgram(0);

	glUseProgram(lightShader);
	mvpLoc = glGetUniformLocation(lightShader, "MVP");
	mLoc = glGetUniformLocation(lightShader, "M");
	unsigned int colorLoc = glGetUniformLocation(lightShader, "objColor");

	N = points.size();
	for (int i = 0; i < N; ++i)
	{
		R_Point p = points[i];
		T = glm::translate(glm::mat4(1.0f), p.pos);
		S = glm::scale(glm::vec3(0.05f));
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
		S = glm::scale(glm::vec3(l.scale, 1.0f, 1.0f));
		M = T * R * S;
		MVP = VP * M;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));
		glUniform3fv(colorLoc, 1, glm::value_ptr(l.color));
		Model m = models[lineModelID];
		glBindVertexArray(m.VAO);
		glDrawArrays(GL_LINES, 0, m.nIndices);
	}

	N = aabbs.size();
	for (int i = 0; i < N; ++i)
	{
		R_aabb aabb = aabbs[i];
		T = glm::translate(glm::mat4(1.0f), (aabb.min + aabb.max) * 0.5f);
		S = glm::scale(glm::abs(aabb.max - aabb.min) * 0.5f);
		M = T * S;
		MVP = VP * M;
		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));
		glUniform3fv(colorLoc, 1, glm::value_ptr(aabb.color));
		Model m = models[cubeModelID];
		glBindVertexArray(m.VAO);
		glLineWidth(2.5f);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
	}
	
	for (int i = 0; i < lightPos.size() - 1; i++)
	{
		T = glm::translate(glm::mat4(1.0f), lightPos[i]);
		S = glm::scale(glm::vec3(0.3f));
		MVP = VP * T * S;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform3fv(colorLoc, 1, glm::value_ptr(lightColors[i]));

		Model m = models[cubeModelID];
		glBindVertexArray(m.VAO);
		glDrawArrays(GL_TRIANGLES, 0, m.nIndices);
	}
	glUseProgram(0);

	points.clear();
	lines.clear();
	aabbs.clear();
	hinges.clear();
}