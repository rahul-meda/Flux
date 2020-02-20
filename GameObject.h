#pragma once

#include <glm/glm.hpp>

class GameObject
{
public:
	GameObject(unsigned int modelID, unsigned int bodyID, glm::vec3 color=glm::vec3(1.0f, 0.7f, 0.4f))
		: modelID(modelID), bodyID(bodyID), color(color)
	{}

	glm::vec3 color;
	unsigned int modelID;
	unsigned int bodyID;	// is it better to store pointers?
};