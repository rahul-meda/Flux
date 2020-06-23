#pragma once

#include "Simulation.h"

class Body;
class HingeJoint;

class Car_Test : public Simulation
{
public:
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyTap(GLFWwindow* window, int key, int scanCode, int action, int mods);
	void OnKeyPress(GLFWwindow* window);
	bool IsGripping();
	void Update(GLFWwindow* window);

	Car_Test() {};
	~Car_Test() {};

private:
	float motorSpeed;		// affects wheel angular speed and hence car linear speed
	float maxMotorTorque;	// limits max speed
	float mass;
	float steerFactor;
	float maxSteerTorque;
	glm::vec3 fwd, up, right;
	float grip;
	bool turning;
	float drag;

	std::vector<Body*> wheels;
	Body* chassis;
	Transform txC;
	std::vector<unsigned int> wheelJointIds;

	glm::vec3 prevPos;
};
