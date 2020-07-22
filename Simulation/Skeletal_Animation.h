#pragma once

#include "Simulation.h"
#include "../Animation/Animation.h"

class Skeletal_Animation : public Simulation
{
public:
	static Skeletal_Animation& GetInstance();
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods);
	void OnKeyPress(GLFWwindow* window);
	void Update(GLFWwindow* window);

	Skeletal_Animation() {};
	~Skeletal_Animation() {};

	Animation walkAnimation;
	Animation runAnimation;
	std::vector<unsigned int> objIds;
	bool pause;
	glm::vec3 pos;
	glm::quat rot;
	glm::mat3 R;
};
