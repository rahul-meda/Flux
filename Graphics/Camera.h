#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class Body;

class Camera
{
public:
	enum DIR {FWD, REV, RIGHT, LEFT, UP, DOWN, IDLE};

	enum TRACK {LINE, CIRCLE, SPLINE};

	Camera();

	Camera(glm::vec3 pos, glm::vec3 fwd, glm::vec3 up);

	glm::vec3 position;
	glm::vec3 velocity;

	glm::vec3 fwd;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 target;
	glm::quat orient;

	bool follow;
	Body* body;
	float radius;		// distance from target
	float minRadius;
	float maxRadius;
	glm::vec3 offset;	// offset of target from body origin
	float yawOff, pitch;
	bool isIdle;

	void Translate(DIR mode);

	// Rotate in place
	void Rotate(const float yaw, const float pitch, const float dx, const float dy);

	// Follow the body
	void Follow(const float dx, const float dy);

	// Camera moves in a sphere defined by center and radius
	// and looking at center
	void RotateSpherical(float yaw, float picth, glm::vec3 center, float radius);

	void Track(TRACK mode);

	glm::mat4 ViewSpace();
};