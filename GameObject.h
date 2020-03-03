#pragma once

#include <glm/glm.hpp>

struct Material
{
	unsigned int diffuseMap;
	unsigned int specularMap;
	unsigned int emissionMap;

	unsigned int count;	// 1, 2 or 3
};

class GameObject
{
public:
	GameObject(unsigned int modelID, unsigned int bodyID, const Material& material)
		: modelID(modelID), bodyID(bodyID), material(material) {}

	unsigned int modelID;
	unsigned int bodyID;	// is it better to store pointers?
	Material material;
};