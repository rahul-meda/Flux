#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <utility>
#include "../Graphics/Texture.h"
#include "../Graphics/Camera.h"

struct MouseInfo
{
	double prevX;
	double prevY;
	double lateral;
	double vertical;

	MouseInfo()
		: prevX(0.0), prevY(0.0), lateral(0.0), vertical(0.0)
	{}
};

class Simulation
{
private:
	Simulation();

	~Simulation();

	GLFWwindow* m_window;

	unsigned int m_shaderID;

	double prevFrame;

public:
	static Simulation& GetInstance();

	void Init(GLFWwindow* window, unsigned int shaderProgram);

	std::map<int, std::pair<int, int>> bufferIDmap;

	TextureInfo textureData;

	Camera camera;

	MouseInfo mouseData;

	void Update();
};
