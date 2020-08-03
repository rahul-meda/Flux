
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

#define INVALID_UNIFORM_LOCATION 0xffffffff

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
	glEnable(GL_CULL_FACE);

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

	stbExtensions = {"jpg", "png", "tga", "bmp", "psd"};
}

void Graphics::PostInit()
{
	GLint max_attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	std::cout << max_attribs << std::endl;

	glUseProgram(worldShader);
	mvpLocW = glGetUniformLocation(worldShader, "MVP");
	mLocW = glGetUniformLocation(worldShader, "M");
	for (int i = 0; i < 3; ++i)
	{
		txLocW[i] = glGetUniformLocation(worldShader, textureLocs[i]);
	}
	eyeLocW = glGetUniformLocation(worldShader, "eyePos");
	lightMapLocW = glGetUniformLocation(worldShader, "lightMap");
	camLightLocW = glGetUniformLocation(worldShader, "lightPos[4]");
	timeLocW = glGetUniformLocation(worldShader, "time");
	glUseProgram(0);

	glUseProgram(animShader);
	mvpLocA = glGetUniformLocation(animShader, "MVP");
	mLocA = glGetUniformLocation(animShader, "M");
	for (int i = 0; i < 3; ++i)
	{
		txLocA[i] = glGetUniformLocation(animShader, textureLocs[i]);
	}
	eyeLocA = glGetUniformLocation(animShader, "eyePos");
	lightMapLocA = glGetUniformLocation(animShader, "lightMap");
	camLightLocA = glGetUniformLocation(animShader, "lightPos[4]");
	timeLocA = glGetUniformLocation(animShader, "time");
	glUseProgram(0);

	AddPointLight(glm::vec3(0.0f));

	glUseProgram(animShader);
	for (unsigned int i = 0; i < MAX_BONES; ++i)
	{
		char name[128];
		memset(name, 0, sizeof(name));
		_snprintf_s(name, sizeof(name), "boneTransforms[%d]", i);
		boneLocs[i] = glGetUniformLocation(animShader, name);
	}
	glUseProgram(0);
}

bool Graphics::STBI_Supported(const std::string& ext)
{
	for (std::string& s : stbExtensions)
	{
		if (s == ext)
			return true;
	}
	return false;
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
	unsigned int sz = sizeof(R_Vertex);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(R_Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(3 * sizeof(float)));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(6 * sizeof(float)));

	glBindVertexArray(0);

	return VAO;
}

unsigned int Graphics::CreateModel(const std::vector<BoneVertex>& vertices, const std::vector<unsigned int>& indices)
{
	unsigned int VAO, VBO, EBO;

	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	unsigned int sz = sizeof(BoneVertex);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BoneVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// set the vertex attribute pointers
	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(3 * sizeof(float)));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(6 * sizeof(float)));
	// bone weights
	for (int i = 0; i < MAX_WEIGHTS; ++i)
	{
		glEnableVertexAttribArray(3 + i);
		glVertexAttribPointer(3 + i, 1, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)((8 + i) * sizeof(float)));
	}
	// bone ids
	for (int i = 0; i < MAX_WEIGHTS; ++i)
	{
		glEnableVertexAttribArray(9 + i);
		glVertexAttribIPointer(9 + i, 1, GL_INT, sizeof(BoneVertex), (void*)(14 * sizeof(float) + i * sizeof(int)));
	}

	glBindVertexArray(0);

	return VAO;
}

void R_Mesh::LoadModel(const R_Mesh& mesh)
{
	VAO = mesh.VAO;
	subMeshes = mesh.subMeshes;
	materials = mesh.materials;
}

void R_Mesh::LoadModel(const std::string& file, bool flip)
{
	VAO = 0;
	nBones = 0;
	subMeshes.clear();
	materials.clear();

	Assimp::Importer importer;
	scene = importer.ReadFile(file,
		aiProcess_Triangulate | aiProcess_GenNormals |
	    aiProcess_FlipUVs     | aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

	AssimpToGlmMat4(scene->mRootNode->mTransformation, invBindTx);
	glm::inverse(invBindTx);

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

	std::vector<R_Vertex> vertices;
	std::vector<BoneVertex> boneVerts;
	std::vector<unsigned int> indices;
	vertices.reserve(nVertices);
	boneVerts.reserve(nVertices);
	indices.reserve(nIndices);

	bool hasAnims = scene->HasAnimations();

	if (!hasAnims)
	{
		LoadGeometry(scene, vertices, indices);
		VAO = Graphics::GetInstance().CreateModel(vertices, indices);
	}
	else
	{
		LoadGeometry(scene, boneVerts, indices);
		/*for (int i = 0; i < nVertices; ++i)
		{
			std::cout << "bone weigths: " << boneVerts[i].boneWeights[0] << "," << boneVerts[i].boneWeights[1]
				<< "," << boneVerts[i].boneWeights[2] << "," << boneVerts[i].boneWeights[3] << std::endl;
			std::cout << "bone IDs: " << boneVerts[i].boneIDs[0] << "," << boneVerts[i].boneIDs[1]
					<< "," << boneVerts[i].boneIDs[2] << "," << boneVerts[i].boneIDs[3] << std::endl;
			if (!boneVerts[i].VerifyWeights())
			{
				std::cout << "invalid bone weigths in vertex: " << i << std::endl;
			}
			if (!boneVerts[i].VerifyBoneIDs())
			{
				std::cout << "invalid bone IDs in vertex: " << i << std::endl;
			}
		}*/
		VAO = Graphics::GetInstance().CreateModel(boneVerts, indices);
	}

	LoadTextures(scene, file, flip);
}

void R_Mesh::LoadGeometry(const aiScene* scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices)
{
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
}

void R_Mesh::LoadGeometry(const aiScene* scene, std::vector<BoneVertex>& vertices, std::vector<unsigned int>& indices)
{
	for (unsigned int i = 0; i < subMeshes.size(); ++i)
	{
		const aiMesh* aiMesh = scene->mMeshes[i];
		for (unsigned int j = 0; j < aiMesh->mNumVertices; ++j)
		{
			BoneVertex v;
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

		LoadBones(i, aiMesh, vertices);
	}
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

				if (p.substr(1, 2) == ":\\")
				{
					slashIndex = p.find_last_of("\\");
					if (slashIndex != std::string::npos)
					{
						p = p.substr(slashIndex + 1, p.size() - 2);
					}
				}

				slashIndex = p.find_last_of("/");
				if (slashIndex != std::string::npos)
				{
					p = p.substr(slashIndex + 1, p.size() - 2);
				}
				std::string::size_type dotIndex = p.find_last_of(".");
				std::string ext = p.substr(dotIndex + 1, p.size() - 2);
				bool stb_supported = false;
				if (!Graphics::GetInstance().STBI_Supported(ext))
				{
					p.replace(dotIndex + 1, ext.size(), "jpg");	// force-load jpg
				}
				
				std::string fullPath = dir + "/" + p;
				m.diffuseMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				nMaps = 1;
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
				nMaps = 2;
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
				nMaps = 3;
			}
		}

		m.nMaps = nMaps;
		materials[i] = m;
	}
}

void R_Mesh::LoadBones(unsigned int meshIndex, const aiMesh* aiMesh, std::vector<BoneVertex>& vertices)
{
	for (unsigned int i = 0; i < aiMesh->mNumBones; ++i)
	{
		unsigned int boneIndex = 0;
		std::string boneName(aiMesh->mBones[i]->mName.data);
		
		if (boneMap.find(boneName) == boneMap.end()) 
		{
			boneIndex = nBones;
			nBones++;
			glm::mat4 boneOffset;
			AssimpToGlmMat4(aiMesh->mBones[i]->mOffsetMatrix, boneOffset);
			boneOffsets.push_back(glm::mat4(1.0f));
			boneOffsets[boneIndex] = boneOffset;
			boneMap[boneName] = boneIndex;
		}
		else 
		{
			boneIndex = boneMap[boneName];
		}

		for (unsigned int j = 0; j < aiMesh->mBones[i]->mNumWeights; j++) 
		{
			unsigned int vertexID = subMeshes[meshIndex].vertexOffset + aiMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = aiMesh->mBones[i]->mWeights[j].mWeight;
			vertices[vertexID].AssignWeights(boneIndex, weight);
		}
	}
}

void AssimpToGlmMat4(const aiMatrix4x4& from, glm::mat4& to)
{
	to[0][0] = (float)from.a1; to[0][1] = (float)from.b1; to[0][2] = (float)from.c1; to[0][3] = (float)from.d1;
	to[1][0] = (float)from.a2; to[1][1] = (float)from.b2; to[1][2] = (float)from.c2; to[1][3] = (float)from.d2;
	to[2][0] = (float)from.a3; to[2][1] = (float)from.b3; to[2][2] = (float)from.c3; to[2][3] = (float)from.d3;
	to[3][0] = (float)from.a4; to[3][1] = (float)from.b4; to[3][2] = (float)from.c4; to[3][3] = (float)from.d4;
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

void Graphics::SetBoneTransform(const int i, const glm::mat4& transform)
{
	glUseProgram(animShader);
	glUniformMatrix4fv(boneLocs[i], 1, GL_FALSE, glm::value_ptr(transform));
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
	glm::vec3 eye = camera.position;
	glUniform3fv(eyeLocW, 1, glm::value_ptr(eye));
	glUniform3fv(camLightLocW, 1, glm::value_ptr(eye));
	glUniform1f(timeLocW, glfwGetTime());

	std::vector<Transform>* transforms = &Physics::GetInstance().transforms;

	int N = objects.size();
	for (int i = 0; i < N; i++)
	{
		R_Mesh m = objects[i];
		int nSub = m.subMeshes.size();
		Transform tx = (*transforms)[m.txID];

		for (int j = 0; j < nSub; ++j)
		{
			T = glm::translate(glm::mat4(1.0f), tx.position);
			R = glm::mat4(tx.R);
			S = glm::scale(m.scale);
			M = T * R * S;
			MVP = VP * M;

			glUniformMatrix4fv(mvpLocW, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(mLocW, 1, GL_FALSE, glm::value_ptr(M));

			unsigned int mID = m.subMeshes[j].materialID;
			assert(mID < m.materials.size());
			Material material = m.materials[mID];
			int nMaps = material.nMaps;
			for (int k = 0; k < nMaps; ++k)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, material.GetMap(k));
				glUniform1i(txLocW[k], k);
			}
			glActiveTexture(GL_TEXTURE0);

			glm::vec3 lightMap(1.0f);
			if (nMaps == 1)
				lightMap.y = lightMap.z = 0.0f;
			else if (nMaps == 2)
				lightMap.z = 0.0f;

			glUniform3fv(lightMapLocW, 1, glm::value_ptr(lightMap));
			glBindVertexArray(m.VAO);
			glDrawElementsBaseVertex(GL_TRIANGLES,
									m.subMeshes[j].nIndices, 
									GL_UNSIGNED_INT,
									(void*)(sizeof(unsigned int) * m.subMeshes[j].indexOffset),
									m.subMeshes[j].vertexOffset);
		}
	}
	glUseProgram(0);

	glUseProgram(animShader);
	glUniform3fv(eyeLocA, 1, glm::value_ptr(eye));
	glUniform3fv(camLightLocA, 1, glm::value_ptr(eye));
	glUniform1f(timeLocA, glfwGetTime());

	N = animModels.size();
	for (int i = 0; i < N; i++)
	{
		R_Mesh m = animModels[i];
		int nSub = m.subMeshes.size();
		Transform tx = (*transforms)[m.txID];

		for (int j = 0; j < nSub; ++j)
		{
			T = glm::translate(glm::mat4(1.0f), tx.position);
			R = glm::mat4(tx.R);
			S = glm::scale(m.scales[0]);
			M = T * R * S;
			MVP = VP * M;

			glUniformMatrix4fv(mvpLocA, 1, GL_FALSE, glm::value_ptr(MVP));
			glUniformMatrix4fv(mLocA, 1, GL_FALSE, glm::value_ptr(M));

			unsigned int mID = m.subMeshes[j].materialID;
			assert(mID < m.materials.size());
			Material material = m.materials[mID];
			int nMaps = material.nMaps;
			for (int k = 0; k < nMaps; ++k)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, material.GetMap(k));
				glUniform1i(txLocA[k], k);
			}
			glActiveTexture(GL_TEXTURE0);

			glm::vec3 lightMap(1.0f);
			if (nMaps == 1)
				lightMap.y = lightMap.z = 0.0f;
			else if (nMaps == 2)
				lightMap.z = 0.0f;

			glUniform3fv(lightMapLocA, 1, glm::value_ptr(lightMap));
			glBindVertexArray(m.VAO);
			glDrawElementsBaseVertex(GL_TRIANGLES,
									m.subMeshes[j].nIndices,
									GL_UNSIGNED_INT,
									(void*)(sizeof(unsigned int) * m.subMeshes[j].indexOffset),
									m.subMeshes[j].vertexOffset);
		}
	}
	glUseProgram(0);

	glUseProgram(worldShader);
	N = hinges.size();
	for (int i = 0; i < N; ++i)
	{
		R_Hinge h = hinges[i];

		T = glm::translate(glm::mat4(1.0f), h.pos);
		R = glm::mat4(h.rot);
		S = glm::scale(glm::vec3(0.25f, h.scale, 0.25f));
		M = T * R * S;
		MVP = VP * M;

		glUniformMatrix4fv(mvpLocW, 1, GL_FALSE, glm::value_ptr(MVP));
		glUniformMatrix4fv(mLocW, 1, GL_FALSE, glm::value_ptr(M));
	
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

		glUniform3fv(lightMapLocW, 1, glm::value_ptr(lightMap));
		glBindVertexArray(dCylinder.VAO);
		glLineWidth(2.0f);
		glDrawArrays(GL_TRIANGLES, 0, dCylinder.subMeshes[0].nIndices);
	}
	glUseProgram(0);

	glUseProgram(lightShader);
	unsigned int mvpLoc = glGetUniformLocation(lightShader, "MVP");
	unsigned int mLoc = glGetUniformLocation(lightShader, "M");
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
		//glDrawArrays(GL_LINES, 0, dLine.subMeshes[0].nIndices);
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
	glUseProgram(0);

	points.clear();
	lines.clear();
	aabbs.clear();
	hinges.clear();
}