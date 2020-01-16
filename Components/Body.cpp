#include "Body.h"

Body::Body(const BodyDef& bd)
{
	tx = bd.tx;
	velocity = bd.velocity;
	angularVelocity = bd.angularVelocity;

	force = glm::vec3(0.0f);
	torque = glm::vec3(0.0f);
}