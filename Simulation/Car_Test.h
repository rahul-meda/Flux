#pragma once

#include "Simulation.h"

class Body;
class HingeJoint;

class Car_Test : public Simulation
{
public:
	void Init(GLFWwindow* window, int width, int height);
	void OnKeyPress(GLFWwindow* window);

	Car_Test() {};
	~Car_Test() {};

private:
	float motorSpeed;		// affects wheel angular speed and hence car linear speed
	float maxMotorTorque;	// limits max speed
	float torqueFactor;
	float steerFactor;

	std::vector<Body*> wheels;
	Body* chassis;

	std::vector<unsigned int> wheelJointIds;

	unsigned int r_wheelIds[2];	// front wheels render object ids
};
