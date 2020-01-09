#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <utility>
#include "../Graphics/Camera.h"
#include "../Graphics/Texture.h"
#include "../Components/Transform.h"
#include "../GameObject.h"

struct MouseInfo
{
	enum Mode {PAN_LEFT, PAN_RIGHT, PAN_UP, PAN_DOWN, NORMAL};
	double prevX;
	double prevY;
	double lateral;
	double vertical;
	float sensitivity;
	Mode pan;

	MouseInfo()
		: prevX(0.0), prevY(0.0), lateral(0.0), vertical(0.0), sensitivity(0.1f), pan(NORMAL)
	{}
};

class Simulation
{
private:
	Simulation();

	~Simulation();

	double prevFrame;

public:
	static Simulation& GetInstance();

	void Init(GLFWwindow* window);

	std::map<int, std::pair<int, int>> bufferIDmap;

	TextureInfo textureData;

	Camera camera;

	MouseInfo mouseData;

	std::vector<GameObject> gameObjects;

	std::vector<Transform> transforms;

	void Update(GLFWwindow* window);
};
