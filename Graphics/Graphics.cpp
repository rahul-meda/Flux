
#ifndef GLFW_INCLUDE_GLEXT
#define GLFW_INCLUDE_GLEXT
#endif
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
#include "Shader.h"
#include "../Simulation/Simulation.h"
#include "../Physics/Physics.h"
#include "../Components/Transform.h"
#include "../Mesh/Geometry.h"
#include "../Mesh/ObjParser.h"
#include "World.h"

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
	glDepthFunc(GL_LEQUAL);    // interpret smaller values as closer
	//glEnable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);
	//glEnable(GL_FRAMEBUFFER_SRGB);

	std::vector<R_Vertex> vertices;
	std::vector<unsigned int> indices;

	dCube.LoadModel("resources/models/box/box.obj");
	dLine.LoadModel("resources/models/line/line.obj");
	dSphere.LoadModel("resources/models/sphere/sphere.obj");
	dCylinder.LoadModel("resources/models/cylinder/cylinder.obj");
	dCapsule.LoadModel("resources/models/capsule/capsule.obj");

	boxVAO  = CreateBox();
	quadVAO = CreateQuad();
	const std::string path = "resources/textures/hdr/Topanga_Forest/Topanga_Forest_B_3k.hdr";
	CreateEnvironmentMap(path, envCubeMap, irradianceMap, prefilterMap, brdfLUTTexture);

	terrainVAO = CreateTerrain();
	terrainMaterial = CreateMaterial("resources/textures/grass1");

	textureLocs[0] = "albedoMap";
	textureLocs[1] = "specularMap";
	textureLocs[2] = "normalMap";
	textureLocs[3] = "glossMap";
	textureLocs[4] = "occlusionMap";

	unsigned int hingeTexture = CreateTexture("resources/textures/metal2.jpg");

	lightColors[0] = glm::vec3(1.0f);
	lightColors[1] = glm::vec3(1.0f);
	lightColors[2] = glm::vec3(1.0f);
	lightColors[3] = glm::vec3(1.0f);

	stbExtensions = {"jpg", "png", "tga", "bmp", "psd"};

	worldShader    = Shader::CreateShader("Resources/WorldVertexShader.vert", "Resources/WorldFragmentShader.frag");
	animShader     = Shader::CreateShader("Resources/AnimVertexShader.vert", "Resources/WorldFragmentShader.frag");
	instanceShader = Shader::CreateShader("Resources/InstanceVertexShader.vert", "Resources/InstanceFragmentShader.frag");
	skyboxShader   = Shader::CreateShader("Resources/Skybox.vert", "Resources/Skybox.frag");
}

void Graphics::PostInit()
{
	GLint max_attribs;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &max_attribs);
	std::cout << max_attribs << std::endl;

	glUseProgram(skyboxShader);
	glUniform1i(glGetUniformLocation(skyboxShader, "environmentMap"), 0);
	glUseProgram(0);

	CreateSkybox(skyboxVAO, skyboxTexture);

	AddPointLight(glm::vec3(0.0f));

	glUseProgram(skyboxShader);
	vpLocS = glGetUniformLocation(skyboxShader, "VP");
	glUniform1i(glGetUniformLocation(skyboxShader, "environmentMap"), 0);
	glUseProgram(0);

	glUseProgram(worldShader);
	mvpLocW = glGetUniformLocation(worldShader, "MVP");
	mLocW = glGetUniformLocation(worldShader, "M");
	for (int i = 0; i < 5; ++i)
	{
		txLocW[i] = glGetUniformLocation(worldShader, textureLocs[i]);
	}
	eyeLocW = glGetUniformLocation(worldShader, "eyePos");
	camLightLocW = glGetUniformLocation(worldShader, "lightPos[4]");
	glUseProgram(0);

	glUseProgram(animShader);
	mvpLocA = glGetUniformLocation(animShader, "MVP");
	mLocA = glGetUniformLocation(animShader, "M");
	for (int i = 0; i < 5; ++i)
	{
		txLocA[i] = glGetUniformLocation(animShader, textureLocs[i]);
	}
	eyeLocA = glGetUniformLocation(animShader, "eyePos");
	camLightLocA = glGetUniformLocation(animShader, "lightPos[4]");
	glUseProgram(0);

	glUseProgram(worldShader);
	glUniform1i(glGetUniformLocation(worldShader, "irradianceMap"), 5);
	glUniform1i(glGetUniformLocation(worldShader, "prefilterMap"), 6);
	glUniform1i(glGetUniformLocation(worldShader, "brdfLUT"), 7);
	glUseProgram(0);

	glUseProgram(animShader);
	glUniform1i(glGetUniformLocation(animShader, "irradianceMap"), 5);
	glUniform1i(glGetUniformLocation(animShader, "prefilterMap"), 6);
	glUniform1i(glGetUniformLocation(animShader, "brdfLUT"), 7);
	glUseProgram(0);

	glUseProgram(instanceShader);
	vpLocI = glGetUniformLocation(instanceShader, "VP");
	glUseProgram(0);

	glUseProgram(animShader);
	for (unsigned int i = 0; i < MAX_BONES; ++i)
	{
		char name[128];
		memset(name, 0, sizeof(name));
		_snprintf_s(name, sizeof(name), "boneTransforms[%d]", i);
		boneLocs[i] = glGetUniformLocation(animShader, name);
	}
	glUseProgram(0);

	grass.Init();
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(offsetof(R_Vertex, R_Vertex::position)));
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(offsetof(R_Vertex, R_Vertex::normal)));
	// vertex tangents
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(offsetof(R_Vertex, R_Vertex::tangent)));
	// vertex bi-tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(offsetof(R_Vertex, R_Vertex::biTangent)));
	// vertex texture coords
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(R_Vertex), (void*)(offsetof(R_Vertex, R_Vertex::textureCoords)));

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::position));
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::normal));
	// vertex tangents
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::tangent));
	// vertex bi-tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::biTangent));
	// bone weights
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::boneWeights));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)(offsetof(BoneVertex, BoneVertex::boneWeights) + 3 * sizeof(float)));
	// bone IDs
	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 3, GL_INT, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::boneIDs));
	glEnableVertexAttribArray(7);
	glVertexAttribIPointer(7, 3, GL_INT, sizeof(BoneVertex), (void*)(offsetof(BoneVertex, BoneVertex::boneIDs) + 3 * sizeof(int)));
	// vertex texture coords
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(BoneVertex), (void*)offsetof(BoneVertex, BoneVertex::textureCoords));

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
	boneMap.clear();
	boneOffsets.clear();

	Assimp::Importer importer;
	scene = importer.ReadFile(file,
		aiProcess_Triangulate | aiProcess_GenNormals |
	    aiProcess_FlipUVs     | aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure |
		aiProcess_CalcTangentSpace);

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
			v.tangent = glm::vec3(aiMesh->mTangents[j].x, aiMesh->mTangents[j].y, aiMesh->mTangents[j].z);
			v.biTangent = glm::vec3(aiMesh->mBitangents[j].x, aiMesh->mBitangents[j].y, aiMesh->mBitangents[j].z);
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
			v.tangent = glm::vec3(aiMesh->mTangents[j].x, aiMesh->mTangents[j].y, aiMesh->mTangents[j].z);
			v.biTangent = glm::vec3(aiMesh->mBitangents[j].x, aiMesh->mBitangents[j].y, aiMesh->mBitangents[j].z);
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

std::string R_Mesh::GetTexturePath(std::string& file)
{
	if (file.substr(0, 2) == ".\\")
	{
		file = file.substr(2, file.size() - 2);
	}
	unsigned int slashIndex;
	if (file.substr(1, 2) == ":\\")
	{
		slashIndex = file.find_last_of("\\");
		if (slashIndex != std::string::npos)
		{
			file = file.substr(slashIndex + 1, file.size() - 2);
		}
	}

	slashIndex = file.find_last_of("/");
	if (slashIndex != std::string::npos)
	{
		file = file.substr(slashIndex + 1, file.size() - 2);
	}
	std::string::size_type dotIndex = file.find_last_of(".");
	std::string ext = file.substr(dotIndex + 1, file.size() - 2);
	bool stb_supported = false;
	if (!Graphics::GetInstance().STBI_Supported(ext))
	{
		file.replace(dotIndex + 1, ext.size(), "jpg");	// force-load jpg
	}

	return file;
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
		
		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0 || pMaterial->GetTextureCount(aiTextureType_BASE_COLOR) > 0)
		{
			if ( (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) ||
			     (pMaterial->GetTexture(aiTextureType_BASE_COLOR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) )
			{
				std::string p = GetTexturePath(std::string(path.data));
				std::string fullPath = dir + "/" + p;
				m.albedoMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip, true);
				++nMaps;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_SPECULAR) > 0 || pMaterial->GetTextureCount(aiTextureType_METALNESS))
		{
			if ( (pMaterial->GetTexture(aiTextureType_SPECULAR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) ||
				 (pMaterial->GetTexture(aiTextureType_METALNESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) )
			{
				std::string p = GetTexturePath(std::string(path.data));
				std::string fullPath = dir + "/" + p;
				m.specularMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				++nMaps;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0 || pMaterial->GetTextureCount(aiTextureType_NORMAL_CAMERA) > 0)
		{
			if ( (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) ||
				 (pMaterial->GetTexture(aiTextureType_NORMAL_CAMERA, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) )
			{
				std::string p = GetTexturePath(std::string(path.data));
				std::string fullPath = dir + "/" + p;
				m.normalMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				++nMaps;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_SHININESS) > 0 || pMaterial->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0)
		{
			if ((pMaterial->GetTexture(aiTextureType_SHININESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) ||
				(pMaterial->GetTexture(aiTextureType_DIFFUSE_ROUGHNESS, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS))
			{
				std::string p = GetTexturePath(std::string(path.data));
				std::string fullPath = dir + "/" + p;
				m.glossMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				++nMaps;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_LIGHTMAP) > 0 || pMaterial->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0)
		{
			if ((pMaterial->GetTexture(aiTextureType_LIGHTMAP, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) ||
				(pMaterial->GetTexture(aiTextureType_AMBIENT_OCCLUSION, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS))
			{
				std::string p = GetTexturePath(std::string(path.data));
				std::string fullPath = dir + "/" + p;
				m.occlusionMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				++nMaps;
			}
		}
		if (pMaterial->GetTextureCount(aiTextureType_EMISSIVE) > 0 || pMaterial->GetTextureCount(aiTextureType_EMISSION_COLOR) > 0)
		{
			if ((pMaterial->GetTexture(aiTextureType_EMISSIVE, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) ||
				(pMaterial->GetTexture(aiTextureType_EMISSION_COLOR, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS))
			{
				std::string p = GetTexturePath(std::string(path.data));
				std::string fullPath = dir + "/" + p;
				m.emissionMap = Graphics::GetInstance().CreateTexture(fullPath.c_str(), flip);
				++nMaps;
			}
		}
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

void Grass::Init()
{
	std::vector<R_Vertex> vertices;
	std::vector<unsigned int> indices;

	//for (int i = 0; i < 6; ++i)
	{
		R_Vertex v;
		v.position = glm::vec3(0.0f, 0.5f, 0.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.textureCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(v);
		v.position = glm::vec3(0.0f, -0.5f, 0.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.textureCoords = glm::vec2(0.0f, 1.0f);
		vertices.push_back(v);
		v.position = glm::vec3(1.0f, -0.5f, 0.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.textureCoords = glm::vec2(1.0f, 1.0f);
		vertices.push_back(v);
		v.position = glm::vec3(0.0f, 0.5f, 0.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.textureCoords = glm::vec2(0.0f, 0.0f);
		vertices.push_back(v);
		v.position = glm::vec3(1.0f, -0.5f, 0.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.textureCoords = glm::vec2(1.0f, 1.0f);
		vertices.push_back(v);
		v.position = glm::vec3(1.0f, 0.5f, 0.0f);
		v.normal = glm::vec3(0.0f, 0.0f, 1.0f);
		v.textureCoords = glm::vec2(1.0f, 0.0f);
		vertices.push_back(v);
	}

	for (int i = 0; i < 6; ++i)
	{
		indices.push_back(++i);
	}

	VAO = Graphics::GetInstance().CreateModel(vertices, indices);

	texture = Graphics::GetInstance().CreateTexture("resources/textures/grass.png");

	for (int i = 0; i < 6; ++i)
	{
		for (int j = 0; j < 6; ++j)
		{
			glm::vec3 p = glm::vec3(10.0f + 2.0f * (float)(i), 0.5f, 2.0f * (float)j);
			glm::mat4 tx = glm::translate(glm::mat4(1.0f), p);
			transforms.push_back(tx);
		}
	}
}

unsigned int Graphics::CreateTexture(const char* filePath, bool flip, bool gammaCorrection)
{
	unsigned int texture;
	glGenTextures(1, &texture);

	int width, height, nrChannels;
	stbi_set_flip_vertically_on_load(flip);

	unsigned char* data = stbi_load(filePath, &width, &height, &nrChannels, 0);
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

	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, gammaCorrection ? GL_SRGB : format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);

	if (!glfwExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{
		std::cout << "Anisotropic filtering not supported" << std::endl;
	}
	GLfloat maxAF;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAF);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAF);

	stbi_image_free(data);

	return texture;
}

Material Graphics::CreateMaterial(const std::string & path, const char* ext, bool flip)
{
	Material material;
	material.albedoMap    = CreateTexture((path + "/albedo" + ext).c_str(), flip, true);
	material.specularMap  = CreateTexture((path + "/specular" + ext).c_str(), flip);
	material.normalMap    = CreateTexture((path + "/normal" + ext).c_str(), flip);
	material.glossMap     = CreateTexture((path + "/gloss" + ext).c_str(), flip);
	material.occlusionMap = CreateTexture((path + "/ao" + ext).c_str(), flip);

	return material;
}

void Graphics::CreateEnvironment(const std::string & envPath)
{
	CreateEnvironmentMap(envPath, envCubeMap, irradianceMap, prefilterMap, brdfLUTTexture);
}

void Graphics::AddPointLight(glm::vec3 pos)
{
	lightPos.push_back(pos);
	int i = lightPos.size() - 1;
	std::string loc = "lightPos[" + std::to_string(i) + "]";

	glUseProgram(worldShader);
	glUniform3fv(glGetUniformLocation(worldShader, loc.c_str()), 1, glm::value_ptr(lightPos[i]));
	glUseProgram(0);
	glUseProgram(animShader);
	glUniform3fv(glGetUniformLocation(animShader, loc.c_str()), 1, glm::value_ptr(lightPos[i]));
	glUseProgram(0);
}

void Graphics::SetBoneTransform(const int i, const glm::mat4& transform)
{
	glUseProgram(animShader);
	glUniformMatrix4fv(boneLocs[i], 1, GL_FALSE, glm::value_ptr(transform));
	glUseProgram(0);
}

void Graphics::Update()
{
	glClearColor(0.125f, 0.125f, 0.125f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glm::mat4 T, R, S, M, VP, MVP;
	VP = P * V;
	
	if (Physics::GetInstance().debugDraw)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1, 0);
	}
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	glUseProgram(worldShader);
	glUniform3fv(eyeLocW, 1, glm::value_ptr(eye));
	glUniform3fv(camLightLocW, 1, glm::value_ptr(eye));

	// bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

	M = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(mvpLocW, 1, GL_FALSE, glm::value_ptr(VP * M));
	glUniformMatrix4fv(mLocW, 1, GL_FALSE, glm::value_ptr(M));
	for (int k = 0; k < 5; ++k)
	{
		glActiveTexture(GL_TEXTURE0 + k);
		glBindTexture(GL_TEXTURE_2D, terrainMaterial.GetMap(k));
		glUniform1i(txLocW[k], k);
	}
	glBindVertexArray(terrainVAO);
	glDrawElements(GL_TRIANGLES, 6 * 255 * 255, GL_UNSIGNED_INT, (void*)(0));

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
			for (int k = 0; k < 5; ++k)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, material.GetMap(k));
				glUniform1i(txLocW[k], k);
			}
			glActiveTexture(GL_TEXTURE0);

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

	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_CUBE_MAP, prefilterMap);
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, brdfLUTTexture);

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
			for (int k = 0; k < 5; ++k)
			{
				glActiveTexture(GL_TEXTURE0 + k);
				glBindTexture(GL_TEXTURE_2D, material.GetMap(k));
				glUniform1i(txLocA[k], k);
			}
			glActiveTexture(GL_TEXTURE0);

			glBindVertexArray(m.VAO);
			glDrawElementsBaseVertex(GL_TRIANGLES,
									m.subMeshes[j].nIndices,
									GL_UNSIGNED_INT,
									(void*)(sizeof(unsigned int) * m.subMeshes[j].indexOffset),
									m.subMeshes[j].vertexOffset);
		}
	}
	glUseProgram(0);

	glUseProgram(instanceShader);
	glUniform3fv(glGetUniformLocation(instanceShader, "eyePos"), 1, glm::value_ptr(eye));
	glUniform3fv(glGetUniformLocation(instanceShader, "lightPos[4]"), 1, glm::value_ptr(eye));
	glUniformMatrix4fv(vpLocI, 1, GL_FALSE, glm::value_ptr(VP));
	N = instModels.size();
	for (int i = 0; i < N; ++i)
	{
		I_Mesh m = instModels[i];
		int nSub = m.subMeshes.size();
		unsigned int mID = m.subMeshes[i].materialID;
		Material material = m.materials[mID];
		glUniform1i(glGetUniformLocation(instanceShader, "diffuseTexture"), 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material.GetMap(0));

		for (int j = 0; j < nSub; ++j)
		{
			glBindVertexArray(m.VAO);
			glDrawElementsInstancedBaseVertex(GL_TRIANGLES,
				m.subMeshes[j].nIndices,
				GL_UNSIGNED_INT,
				(void*)(sizeof(unsigned int) * m.subMeshes[j].indexOffset),
				m.instanceCount,
				m.subMeshes[j].vertexOffset);
		}
	}
	glUseProgram(0);

	glUseProgram(worldShader);
	glBindVertexArray(grass.VAO);
	glBindTexture(GL_TEXTURE_2D, grass.texture);
	glUniform1i(txLocA[0], 0);
	glm::vec3 lightMap(1.0f, 0.0f, 0.0f);
	glUniform3fv(lightMapLocW, 1, glm::value_ptr(lightMap));
	N = grass.transforms.size();
	for (int i = 0; i < N; ++i)
	{
		MVP = VP * grass.transforms[i];
		glUniformMatrix4fv(mvpLocW, 1, GL_FALSE, glm::value_ptr(MVP));
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	glBindVertexArray(0);

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
		glBindTexture(GL_TEXTURE_2D, 0);
		glUniform1i(glGetUniformLocation(worldShader, textureLocs[0]), 0);

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

	glUseProgram(skyboxShader);
	// draw skybox as last
	//glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
	VP = P * glm::mat4(glm::mat3(V)); // remove translation from the view matrix
	glUniformMatrix4fv(vpLocS, 1, GL_FALSE, glm::value_ptr(VP));
	// skybox cube
	glBindVertexArray(boxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, envCubeMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	//glDepthFunc(GL_LESS); // set depth function back to default
	glUseProgram(0);

	points.clear();
	lines.clear();
	aabbs.clear();
	hinges.clear();
}