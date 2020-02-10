#pragma once

#include <glm/glm.hpp>

class GameObject
{
public:
	GameObject(unsigned int modelID, unsigned int bodyID, glm::vec3 color = glm::vec3(0.0f, 0.67f, 1.0f))
		: modelID(modelID), bodyID(bodyID), color(color)
	{}

	glm::vec3 color;
	unsigned int modelID;
	unsigned int bodyID;	// is it better to store pointers?
};