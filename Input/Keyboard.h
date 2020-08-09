#pragma once

#include <GLFW/glfw3.h>
#include <vector>

class Keyboard
{
public:
	Keyboard();
	bool IsKeyDown(const unsigned int key) const;
	bool WasKeyDown(const unsigned int key) const;
	bool IsKeyUp(const unsigned int key) const;
	bool WasKeyUp(const unsigned int key) const;
	bool IsKeyReleased(const unsigned int key) const;
	void Update(GLFWwindow* window);
private:
	std::vector<bool> keysState;
	std::vector<bool> keysStatePrev;
};