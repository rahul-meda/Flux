#pragma once

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../GameObject.h"

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

class Model;
struct TextureInfo;
struct ModelDef;
class GameObject;

class Graphics
{
private:
	Graphics();

public:
	static Graphics& GetInstance();

	void Initialize();

	unsigned int CreateModel(const ModelDef& modelDef);

	unsigned int CreateTexture(const char* filePath);

	void AddPointLight(glm::vec3 pos);

	void Update(const std::vector<GameObject>& objects);

	glm::mat4 P;

	std::vector<Model> models;
	std::vector<glm::vec3> scales;
	std::vector<glm::vec3> lightPos;

	// debug draw
	std::vector<R_Point> points;
	std::vector<R_Line> lines;
	std::vector<R_aabb> aabbs;

	unsigned int worldShader;
	unsigned int lightShader;

	unsigned int cubeModelID;
	unsigned int lineModelID;
	unsigned int normalsModelID;
	unsigned int tangentsModelID;
	unsigned int jointsModelID;
	unsigned int cylinderModelID;

	Material hingeMaterial;
	std::vector<R_Hinge> hinges;

	char* textureLocs[3];
	glm::vec3 lightColors[4];
};