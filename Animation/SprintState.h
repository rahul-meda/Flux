#pragma once

#include "AnimState.h"

#define SPRINT_SPEED 10.0f

class SprintState : public AnimState
{
public:
	SprintState(Body* body)
	: AnimState(body) {}

	bool Trigger(Transition& trans) override
	{
		if (trans == T_IDLE)
			return true;
		if (trans == T_RUN)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = SPRINT + 1;
	}

	void OnExit() override
	{}

	void Update(Transition& transID) override
	{
		glm::vec3 fwd = body->GetTransform().R[2];
		body->SetVelocity(SPRINT_SPEED * fwd);
	}
};