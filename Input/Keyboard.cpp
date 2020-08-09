
#include "Keyboard.h"

Keyboard::Keyboard()
{
	keysState = std::vector<bool>(GLFW_KEY_LAST);
	keysStatePrev = std::vector<bool>(GLFW_KEY_LAST);
}

bool Keyboard::IsKeyDown(const unsigned int key) const
{
	return keysState[key];
}

bool Keyboard::WasKeyDown(const unsigned int key) const
{
	return keysStatePrev[key];
}

bool Keyboard::IsKeyUp(const unsigned int key) const
{
	return !keysState[key];
}

bool Keyboard::WasKeyUp(const unsigned int key) const
{
	return !keysStatePrev[key];
}

bool Keyboard::IsKeyReleased(const unsigned int key) const
{
	return (keysStatePrev[key] && !keysState[key]);
}

void Keyboard::Update(GLFWwindow* window)
{
	keysStatePrev = keysState;

	for (int i = 0; i < GLFW_KEY_LAST; ++i)
	{
		keysState[i] = glfwGetKey(window, i);
	}
}