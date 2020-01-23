#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <utility>
#include "../Graphics/Camera.h"
#include "../Graphics/Texture.h"
#include "../Components/Transform.h"
#include "../GameObject.h"
#include "../Physics/BroadPhase.h"

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

	void Init(GLFWwindow* window, int width, int height);

	void OnKeyPress(GLFWwindow* window, int key, int scanCode, int action, int mods);	// single key press

	void OnKeyPressHold(GLFWwindow* window);

	void OnWindowResize(GLFWwindow* window, int width, int height);

	void OnMouseMove(GLFWwindow* window, double x, double y);

	void Update(GLFWwindow* window);

	int width;
	int height;

	std::map<int, std::pair<int, int>> bufferIDmap;

	TextureInfo textureData;

	Camera camera;

	MouseInfo mouseData;

	BroadPhase bp;

	std::vector<Collider*> colliders;
	std::vector<GameObject> gameObjects;
};
