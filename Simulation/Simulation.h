#pragma once

#include <GLFW/glfw3.h>
#include <vector>
#include <map>
#include <utility>
#include "../Graphics/Camera.h"
#include "../Graphics/Graphics.h"
#include "../Components/Transform.h"
#include "../Animation/AnimFSM.h"
#include "../Input/Keyboard.h"

class AnimState;

struct MouseInfo
{
	enum Mode {PAN_LEFT, PAN_RIGHT, PAN_UP, PAN_DOWN, DEFAULT};
	double prevX;
	double prevY;
	float dx, dy;
	double lateral;
	double vertical;
	float sensitivity;
	Mode pan;

	MouseInfo()
		: prevX(0.0), prevY(0.0), lateral(0.0), vertical(0.0), sensitivity(0.25f), pan(DEFAULT)
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
	void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset);

	void Update(GLFWwindow* window);

	int width;
	int height;

	Camera camera;
	Keyboard keyboard;
	std::map<int, std::pair<int, int>> bufferIDmap;
	MouseInfo mouseData;
	Material material;

	AnimFSM animFSM;
};
