#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "Camera.h"

struct R_Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoords;
};

struct Material
{
	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int emissionMap;
	unsigned int nMaps;
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

struct R_Mesh
{
	glm::vec3 pos;
	glm::mat3 rot;
	std::vector<glm::vec3> posOffsets;
	std::vector<glm::mat3> rotOffsets;
	std::vector<glm::vec3> scales;
	glm::vec3 scale;
	std::vector<unsigned int> VAOs;
	std::vector<unsigned int> nIndices;
	std::vector<Material> materials;
	std::string directory;

	void Clear()
	{
		posOffsets.clear();
		rotOffsets.clear();
		scales.clear();
		VAOs.clear();
		materials.clear();
		nIndices.clear();
		directory = "";
	}

	void ProcessNode(aiNode *node, const aiScene *scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices);
	void ProcessMesh(aiMesh *mesh, const aiScene *scene, std::vector<R_Vertex>& vertices, std::vector<unsigned int>& indices);
	unsigned int CreateTexture(const char* path, bool gamma = false);
	void LoadModel(const std::string& fileName);
};

class Graphics
{
private:
	Graphics();

public:
	static Graphics& GetInstance();
	void Initialize();	// init stuff before any object is added
	void PostInit();	// init stuff after all objects are added
	unsigned int CreateModel(const std::vector<R_Vertex>& vertices, const std::vector<unsigned int>& indices);
	unsigned int CreateTexture(const char* filePath, bool flip = false);
	void AddPointLight(glm::vec3 pos);
	void Update(Camera& camera);

	glm::mat4 P;

	std::vector<R_Mesh> objects;

	std::vector<glm::vec3> lightPos;

	// debug draw
	std::vector<R_Point> points;
	std::vector<R_Line> lines;
	std::vector<R_aabb> aabbs;

	unsigned int worldShader;
	unsigned int lightShader;

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
};