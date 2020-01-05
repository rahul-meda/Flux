#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <utility>
#include "../Graphics/Texture.h"

class Simulation
{
private:
	Simulation();

	~Simulation();

	GLFWwindow* m_window;

	unsigned int m_shaderID;

public:
	static Simulation& GetInstance();

	void Init(GLFWwindow* window, unsigned int shaderProgram);

	std::map<int, std::pair<int, int>> bufferIDmap;

	TextureInfo textureData;

	void Update();
};