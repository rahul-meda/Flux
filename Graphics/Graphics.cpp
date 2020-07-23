
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <iostream>
#include <string>

#include "Graphics.h"
#include "../Simulation/Simulation.h"
#include "../Physics/Physics.h"
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

	std::vector<R_Vertex> vertices;
	std::vector<unsigned int> indices;

	dCube.LoadModel("resources/models/box/box.obj");
	dLine.LoadModel("resources/models/line/line.obj");
	dSphere.LoadModel("resources/models/sphere/sphere.obj");
	dCylinder.LoadModel("resources/models/cylinder/cylinder.obj");
	dCapsule.LoadModel("resources/models/capsule/capsule.obj");

	textureLocs[0] = "diffuseTexture";
	textureLocs[1] = "specularTexture";
	textureLocs[2] = "emissionTexture";

	unsigned int hingeTexture = CreateTexture("resources/textures/metal2.jpg");

	hingeMaterial.diffuseMap = hingeTexture;
	hingeMaterial.specularMap = hingeTexture;
	hingeMaterial.nMaps = 2;

	lightColors[0] = glm::vec3(1.0f, 0.0f, 0.0f);
	lightColors[1] = glm::vec3(0.0f, 1.0f, 0.0f);
	lightColors[2] = glm::vec3(0.0f, 0.0f, 1.0f);
	lightColors[3] = glm::vec3(1.0f, 1.0f, 0.0f);
}

void Graphics::PostInit()
{
	AddPointLight(glm::vec3(0.0f));
}

unsigned int Graphics::CreateModel(const std::vector<R_Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	unsigned int VAO, VBO, EBO;

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(R_Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)offsetof(R_Vertex, textureCoords));

	glBindVertexArray(0);

	return VAO;
}

void R_Mesh::LoadModel(const std::string& file, bool flip)
{
	VAO = 0;
	subMeshes.clear();
	materials.clear();
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	std::vector<R_Vertex> vertices;
	std::vector<unsigned int> indices;
	unsigned int nVertices = 0;
	unsigned int nIndices = 0;

	subMeshes.resize(scene->mNumMeshes);
	materials.resize(scene->mNumMaterials);

	for (unsigned int i = 0; i < subMeshes.size(); ++i) 
	{
		subMeshes[i].materialID = scene->mMeshes[i]->mMaterialIndex;
		subMeshes[i].nIndices = scene->mMeshes[i]->mNumFaces * 3;
		subMeshes[i].vertexOffset = nVertices;
		subMeshes[i].indexOffset = nIndices;

		nVertices += scene->mMeshes[i]->mNumVertices;
		nIndices += subMeshes[i].nIndices;
	}

	vertices.reserve(nVertices);
	indices.reserve(nIndices);

	for (unsigned int i = 0; i < subMeshes.size(); ++i)
	{
		const aiMesh* aiMesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < aiMesh->mNumVertices; ++j) 
		{
			R_Vertex v;
			v.position = glm::vec3(aiMesh->mVertices[j].x, aiMesh->mVertices[j].y, aiMesh->mVertices[j].z);
			v.normal = glm::vec3(aiMesh->mNormals[j].x, aiMesh->mNormals[j].y, aiMesh->mNormals[j].z);
			if (aiMesh->HasTextureCoords(0))
			{
				v.textureCoords = glm::vec2(aiMesh->mTextureCoords[0][j].x, aiMesh->mTextureCoords[0][j].y);
			}
			else
			{
				v.textureCoords = glm::vec2(0.0f);
			}
			vertices.push_back(v);
		}

		for (unsigned int j = 0; j < aiMesh->mNumFaces; ++j) 
		{
			const aiFace& face = aiMesh->mFaces[j];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}
	}

	VAO = Graphics::GetInstance().CreateModel(vertices, indices);

	LoadTextures(scene, file, flip);
}

void R_Mesh::LoadTextures(const aiScene* scene, const std::string& file, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);

	std::string::size_type slashIndex = file.find_last_of("/");
	std::string dir;

	if (slashIndex == std::string::npos)
	{
		dir = ".";
	}
	else if (slashIndex == 0) {
		dir = "/";
	}
	else 
	{
		dir = file.substr(0, slashIndex);
	}

	// Initialize the materials
	if (scene->mNumMaterials > 1)
		std::cout << file << " " << scene->mNumMaterials << std::endl;
	for (unsigned int i = 0; i < scene->mNumMaterials; ++i)
	{
		const aiMaterial* pMaterial = scene->mMaterials[i];
		Material m;
		unsigned int nMaps = 0;
		aiString path;

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string p(path.data);

				if (p.substr(0, 2) == ".\\") 
				{
					p = p.substr(2, p.size() - 2);
				}

				std::string fullPath = dir + "/" + p;
				m.diffuseMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				nMaps++;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0)
		{
			if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string p(path.data);

				if (p.substr(0, 2) == ".\\")
				{
					p = p.substr(2, p.size() - 2);
				}

				std::string fullPath = dir + "/" + p;
				m.specularMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				nMaps++;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0)
		{
			if (pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
			{
				std::string p(path.data);

				if (p.substr(0, 2) == ".\\")
				{
					p = p.substr(2, p.size() - 2);
				}

				std::string fullPath = dir + "/" + p;
				m.emissionMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				nMaps++;
			}
		}

		m.nMaps = nMaps;
		materials[i] = m;
	}
}

unsigned int Graphics::CreateTexture(const char* filePath, bool flip)
{
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flip);

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

void Graphics::Update(Camera& camera)
{
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 T, R, S, M, VP, MVP;
	VP = P * camera.ViewSpace();

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
	glm::vec3 eye = camera.position;
	glUniform3fv(eyeLoc, 1, glm::value_ptr(eye));
	glUniform3fv(glGetUniformLocation(worldShader, "lightPos[4]"), 1, glm::value_ptr(eye));
	glUniform1f(glGetUniformLocation(worldShader, "time"), glfwGetTime());

	int N = objects.size();
	for (int i = 0; i < N; i++)
	{
		R_Mesh obj = objects[i];
		int nSub = obj.subMeshes.size();

		for (int j = 0; j < nSub; ++j)
		{
			T = glm::translate(glm::mat4(1.0f), obj.pos);
			R = glm::mat4(obj.rot);
			S = glm::scale(obj.scales[0]);
			M = T * R * S;
			MVP = VP * M;

			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));

			unsigned int mID = obj.subMeshes[j].materialID;
			//assert(mID < obj.materials.size());
			Material material = obj.materials[mID];
			int nMaps = material.nMaps;
			for (int k = 0; k < nMaps; ++k)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, material.GetMap(k));
				glUniform1i(glGetUniformLocation(worldShader, textureLocs[k]), k);
			}
			glActiveTexture(GL_TEXTURE0);

			glm::vec3 lightMap(1.0f);
			if (nMaps == 1)
				lightMap.y = lightMap.z = 0.0f;
			else if (nMaps == 2)
				lightMap.z = 0.0f;

			glUniform3fv(lightMapLoc, 1, glm::value_ptr(lightMap));
			glBindVertexArray(obj.VAO);
			glDrawElementsBaseVertex(GL_TRIANGLES, obj.subMeshes[j].nIndices, GL_UNSIGNED_INT,
									(void*)(sizeof(unsigned int) * obj.subMeshes[j].indexOffset), obj.subMeshes[j].vertexOffset);
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
	
		unsigned int* mapID = &hingeMaterial.diffuseMap;
		int C = hingeMaterial.nMaps;
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
		glBindVertexArray(dCylinder.VAO);
		glLineWidth(2.0f);
		glDrawArrays(GL_TRIANGLES, 0, dCylinder.subMeshes[0].nIndices);
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
		glBindVertexArray(dCube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, dCube.subMeshes[0].nIndices);
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
		glBindVertexArray(dLine.VAO);
		glDrawArrays(GL_LINES, 0, dLine.subMeshes[0].nIndices);
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
		glBindVertexArray(dCube.VAO);
		glLineWidth(2.5f);
		glDrawArrays(GL_TRIANGLES, 0, dCube.subMeshes[0].nIndices);
	}
	
	for (int i = 0; i < lightPos.size() - 1; i++)
	{
		T = glm::translate(glm::mat4(1.0f), lightPos[i]);
		S = glm::scale(glm::vec3(0.1f));
		MVP = VP * T * S;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform3fv(colorLoc, 1, glm::value_ptr(lightColors[i]));
		glBindVertexArray(dCube.VAO);
		glDrawArrays(GL_TRIANGLES, 0, dCube.subMeshes[0].nIndices);
	}

	points.clear();
	lines.clear();
	aabbs.clear();
	hinges.clear();
}