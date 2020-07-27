#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Camera.h"

#define MAX_BONES 100

struct R_Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

struct BoneVertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
	glm::vec4 boneWeights;
	glm::ivec4 boneIDs;

	BoneVertex() 
	 : boneWeights(glm::vec4(0.0f)), boneIDs(glm::ivec4(0)) {}

	void AssignWeights(unsigned int id, float weight)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (boneWeights[i] == 0.0f)
			{
				boneIDs[i] = id;
				boneWeights[i] = weight;
				return;
			}
		}
		assert(0);
	}

	bool VerifyWeights()
	{
		return (boneWeights[0] + boneWeights[1] + boneWeights[2] + boneWeights[3] == 1.0f);
	}

	bool VerifyBoneIDs()
	{
		if (boneIDs[0] < 0 || boneIDs[0] > 31)
			return false;
		if (boneIDs[1] < 0 || boneIDs[1] > 31)
			return false;
		if (boneIDs[2] < 0 || boneIDs[2] > 31)
			return false;
		if (boneIDs[3] < 0 || boneIDs[3] > 31)
			return false;
		return true;
	}
};

struct Material
{
	Material()
	 : diffuseMap(0), specularMap(0), emissionMap(0), nMaps(0){}

	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int emissionMap;
	unsigned int nMaps;

	unsigned int GetMap(const int i) const
	{
		switch (i)
		{
		case 0:
			return diffuseMap;
		case 1:
			return specularMap;
		case 2:
			return emissionMap;
		}
	}
};

// debug rendering
struct R_Debug
{
	unsigned int VAO;
	unsigned int nIndices;
};

struct R_Point
{
	R_Point(glm::vec3 pos, glm::vec3 color = glm::vec3(1.0f, 1.0f, 0.3f))
		: pos(pos), color(color) {}

	glm::vec3 pos;
	glm::vec3 color;
};

struct R_Line
{
	R_Line(glm::vec3 pos, glm::quat rot, glm::vec3 color = glm::vec3(1.0f, 0.0f, 0.0f), 
		   float scale = 1.0f)
		: pos(pos), rot(rot), color(color), scale(scale) {}

	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 color;
	float scale;
};

struct R_aabb
{
	R_aabb(glm::vec3 min, glm::vec3 max, glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f))
		: min(min), max(max), color(color) {}
	glm::vec3 min;
	glm::vec3 max;
	glm::vec3 color;
};

struct R_Hinge
{
	glm::vec3 pos;
	glm::mat3 rot;
	float scale;
};

struct SubMesh 
{
	SubMesh()
	{
		nIndices = 0;
		vertexOffset = 0;
		indexOffset = 0;
		materialID = 0;
	}

	unsigned int nIndices;
	unsigned int vertexOffset;
	unsigned int indexOffset;
	unsigned int materialID;
};

struct BoneTx
{
	BoneTx()
	{
		offset = glm::mat4(1.0f);
		tx = glm::mat4(1.0f);
	}

	glm::mat4 offset;
	glm::mat4 tx;
};

class R_Mesh
{
public:
	glm::vec3 pos;
	glm::mat3 rot;
	std::vector<glm::vec3> posOffsets;
	std::vector<glm::mat3> rotOffsets;
	std::vector<glm::vec3> scales;
	glm::vec3 scale;
	unsigned int VAO;
	std::vector<SubMesh> subMeshes;
	std::vector<Material> materials;
	unsigned int nBones;
	std::vector<glm::mat4> boneOffsets;
	std::map<std::string, unsigned int> boneMap;
	glm::mat4 invBindTx;
	const aiScene* scene;

	void LoadModel(const std::string& fileName, bool flip = false);
	void LoadGeometry(const aiScene* scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices);
	void LoadGeometry(const aiScene* scene, std::vector<BoneVertex>& vertices, std::vector<unsigned int>& indices);
	void LoadTextures(const aiScene* scene, const std::string& file, bool flip = false);
	void LoadBones(unsigned int meshIndex, const aiMesh* aiMesh, std::vector<BoneVertex>& vertices);

	void Clear()
	{
		posOffsets.clear();
		rotOffsets.clear();
		scales.clear();
	}
};

void AssimpToGlmMat4(const aiMatrix4x4& from, glm::mat4& to);

class Graphics
{
private:
	Graphics();

public:
	static Graphics& GetInstance();
	void Initialize();	// init stuff before any object is added
	void PostInit();	// init stuff after all objects are added
	unsigned int CreateModel(const std::vector<R_Vertex>& vertices, const std::vector<unsigned int>& indices);
	unsigned int CreateModel(const std::vector<BoneVertex>& vertices, const std::vector<unsigned int>& indices);
	unsigned int CreateTexture(const char* filePath, bool flip = false);
	void AddPointLight(glm::vec3 pos);
	void SetBoneTransform(const int i, const glm::mat4& transform);
	void Update(Camera& camera);

	glm::mat4 P;

	std::vector<R_Mesh> objects;
	std::vector<R_Mesh> animModels;

	std::vector<glm::vec3> lightPos;

	// debug draw
	std::vector<R_Point> points;
	std::vector<R_Line> lines;
	std::vector<R_aabb> aabbs;

	unsigned int worldShader;
	unsigned int lightShader;
	unsigned int animShader;

	R_Mesh dCube;
	R_Mesh dLine;
	R_Mesh dSphere;
	R_Mesh dCylinder;
	R_Mesh dCapsule;
	R_Mesh dSoup;

	Material hingeMaterial;
	std::vector<R_Hinge> hinges;

	char* textureLocs[3];
	glm::vec3 lightColors[4];

	unsigned int boneLocs[MAX_BONES];
};