
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

void R_Mesh::LoadModel(const std::string& file)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

	directory = file.substr(0, file.find_last_of('/'));

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	std::vector<R_Vertex> vertices;
	std::vector<unsigned int> indices;

	// process ASSIMP's root node recursively
	ProcessNode(scene->mRootNode, scene, vertices, indices);

	VAOs.push_back(Graphics::GetInstance().CreateModel(vertices, indices));
	nIndices.push_back(indices.size());
}

void R_Mesh::ProcessNode(aiNode *node, const aiScene *scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		ProcessMesh(mesh, scene, vertices, indices);
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene, vertices, indices);
	}
}

void R_Mesh::ProcessMesh(aiMesh *mesh, const aiScene *scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices)
{
	R_Vertex vertex;
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		vertex.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);

		vertex.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		// texture coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.textureCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
		{
			vertex.textureCoords = glm::vec2(0.0f, 0.0f);
		}
		vertices.push_back(vertex);
	}
	
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	Material m;

	for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		aiString str;
		material->GetTexture(aiTextureType_DIFFUSE, i, &str);
		m.diffuseMap = CreateTexture(str.C_Str());
	}

	for (unsigned int i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
	{
		aiString str;
		material->GetTexture(aiTextureType_SPECULAR, i, &str);
		m.specularMap = CreateTexture(str.C_Str());
	}

	m.nMaps = 2;
	materials.push_back(m);
}

unsigned int R_Mesh::CreateTexture(const char* path, bool gamma)
{
	stbi_set_flip_vertically_on_load(false);

	std::string file = directory + '/' + std::string(path);

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(file.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
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
		int nModels = obj.VAOs.size();

		for (int j = 0; j < nModels; ++j)
		{
			T = glm::translate(glm::mat4(1.0f), obj.pos + obj.rot * obj.posOffsets[j]);
			R = glm::mat4(obj.rot * obj.rotOffsets[j]);
			S = glm::scale(obj.scales[j]);
			M = T * R * S;
			MVP = VP * M;

			glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(mLoc, 1, GL_FALSE, glm::value_ptr(M));

			Material material = obj.materials[j];
			unsigned int* mapID = &material.diffuseMap;
			int nMaps = material.nMaps;
			for (unsigned int n = 0; n < nMaps; ++n)
			{
				unsigned int tID = *(mapID + n);
				glActiveTexture(GL_TEXTURE0 + tID - 1);
				glBindTexture(GL_TEXTURE_2D, tID);

				glUniform1i(glGetUniformLocation(worldShader, textureLocs[n]), tID - 1);
			}

			glm::vec3 lightMap(1.0f);
			if (nMaps == 1)
				lightMap.y = lightMap.z = 0.0f;
			else if (nMaps == 2)
				lightMap.z = 0.0f;

			glUniform3fv(lightMapLoc, 1, glm::value_ptr(lightMap));
			glBindVertexArray(obj.VAOs[j]);
			glLineWidth(2.0f);
			glDrawArrays(GL_TRIANGLES, 0, obj.nIndices[j]);
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
		glBindVertexArray(dCylinder.VAOs[0]);
		glLineWidth(2.0f);
		glDrawArrays(GL_TRIANGLES, 0, dCylinder.nIndices[0]);
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
		glBindVertexArray(dCube.VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, dCube.nIndices[0]);
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
		glBindVertexArray(dLine.VAOs[0]);
		glDrawArrays(GL_LINES, 0, dLine.nIndices[0]);
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
		glBindVertexArray(dCube.VAOs[0]);
		glLineWidth(2.5f);
		glDrawArrays(GL_TRIANGLES, 0, dCube.nIndices[0]);
	}
	
	for (int i = 0; i < lightPos.size() - 1; i++)
	{
		T = glm::translate(glm::mat4(1.0f), lightPos[i]);
		S = glm::scale(glm::vec3(0.1f));
		MVP = VP * T * S;

		glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniform3fv(colorLoc, 1, glm::value_ptr(lightColors[i]));
		glBindVertexArray(dCube.VAOs[0]);
		glDrawArrays(GL_TRIANGLES, 0, dCube.nIndices[0]);
	}

	points.clear();
	lines.clear();
	aabbs.clear();
	hinges.clear();
}