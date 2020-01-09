#pragma once

#include <vector>
#include <glm/glm.hpp>

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

	unsigned int CreateModel(const ModelDef& modelDef);

	std::vector<Model> models;

	unsigned int worldShader;

	unsigned int lightShader;

	std::vector<glm::vec3> lightPos;

	unsigned int lightModelID;

	void AddPointLight(glm::vec3 pos);

	glm::mat4 P;

	void Update(const std::vector<GameObject>& objects);
};