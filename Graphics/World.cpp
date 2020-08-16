
#include <glad/glad.h>
#include <iostream>
#include <glm/gtc/random.hpp>
#include <glm/gtx/transform.hpp>
#include "Graphics.h"
#include "World.h"
#include "../stb_image.h"

void CreateSkybox(unsigned int& VAO, unsigned int& textureID)
{
	float vertices[] = 
	{         
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,
		1.0f ,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f , -1.0f, -1.0f,
		1.0f , -1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f , -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f ,  1.0f, -1.0f,
		1.0f ,  1.0f,  1.0f,
		1.0f ,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f , -1.0f, -1.0f,
		1.0f , -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f , -1.0f,  1.0f
	};

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	const char* paths[6] = {"resources/textures/skybox/milky_way/right.png",
							"resources/textures/skybox/milky_way/left.png", 
							"resources/textures/skybox/milky_way/top.png", 
							"resources/textures/skybox/milky_way/bottom.png", 
							"resources/textures/skybox/milky_way/front.png", 
							"resources/textures/skybox/milky_way/back.png"};

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char *data = stbi_load(paths[i], &width, &height, &nrChannels, 0);
		if (!data)
		{
			std::cout << "Cubemap texture failed to load at path: " << paths[i] << std::endl;
			stbi_image_free(data);
		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void CreateAstroidBelt(std::vector<glm::mat4>& transforms, const unsigned int count)
{
	glm::mat4 tx;
	transforms = std::vector<glm::mat4>(count);

	for (int i = 0; i < count; ++i)
	{
		glm::vec3 pos;
		pos.y = 100.0f;
		glm::vec2 c = glm::circularRand(500.0f);
		pos.x = c.x;
		pos.z = c.y;
		float offset = 100.0f;
		pos.x += glm::linearRand(-offset, offset);
		pos.z += glm::linearRand(-offset, offset);
		pos.y += glm::linearRand(-offset * 0.2f, offset * 0.2f);
		glm::mat4 T = glm::translate(glm::mat4(1.0f), pos);

		float angle = glm::linearRand(0.0f, glm::radians(360.0f));
		glm::vec3 axis = glm::sphericalRand(1.0f);
		glm::quat q = glm::angleAxis(angle, axis);
		glm::mat4 R = glm::mat4(q);

		float scale = glm::linearRand(0.1f, 0.5f);
		glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(scale));

		tx = T * R * S;
		transforms[i] = tx;
	}
}

I_Mesh::I_Mesh(const unsigned int instanceCount)
 : instanceCount(instanceCount) {}

void I_Mesh::LoadModel(const std::string & file, bool flip)
{
	VAO = 0;
	subMeshes.clear();
	materials.clear();

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file,
		aiProcess_Triangulate | aiProcess_GenNormals |
		aiProcess_FlipUVs | aiProcess_RemoveRedundantMaterials |
		aiProcess_JoinIdenticalVertices | aiProcess_ValidateDataStructure);

	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
		return;
	}

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
	std::vector<unsigned int> indices;
	vertices.reserve(nVertices);
	indices.reserve(nIndices);

	LoadGeometry(scene, vertices, indices);
	VAO = Graphics::GetInstance().CreateModel(vertices, indices);

	unsigned int VBO2;
	glGenBuffers(1, &VBO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);
	glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(glm::mat4), &(Graphics::GetInstance().instTransforms[0]), GL_STATIC_DRAW);
	glBindVertexArray(VAO);
	// set attribute pointers for matrix (4 times vec4)
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

	glVertexAttribDivisor(3, 1);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glBindVertexArray(0);

	LoadTextures(scene, file, flip);
}

void I_Mesh::LoadGeometry(const aiScene* scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices)
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

void I_Mesh::LoadTextures(const aiScene* scene, const std::string& file, bool flip)
{
	stbi_set_flip_vertically_on_load(flip);

	std::string::size_type slashIndex = file.find_last_of("/");
	std::string dir;

	if (slashIndex == std::string::npos)
	{
		dir = ".";
	}
	else if (slashIndex == 0) 
	{
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