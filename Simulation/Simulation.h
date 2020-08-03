#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <utility>
#include "../Graphics/Camera.h"
#include "../Graphics/Graphics.h"
#include "../Components/Transform.h"
#include "../Animation/AnimFSM.h"

class AnimState;

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
public:
	Simulation();
	~Simulation();

	void Init(GLFWwindow* window, int width, int height);

	void OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods);
	void OnKeyPress(GLFWwindow* window);
	void OnWindowResize(GLFWwindow* window, int width, int height);
	void OnMouseMove(GLFWwindow* window, double x, double y);

	void Update(GLFWwindow* window);

	int width;
	int height;
	double prevFrame;

	Camera camera;
	std::map<int, std::pair<int, int>> bufferIDmap;
	MouseInfo mouseData;
	Material material;

	AnimFSM animFSM;
	std::vector<AnimState*> states;
};
