#pragma once

#include "AnimState.h"

#define RUN_SPEED 5.0f

class RunState : public AnimState
{
public:
	RunState(Body* body)
	: AnimState(body) {}

	bool Trigger(Transition& trans) override
	{
		if (trans == T_IDLE)
			return true;
		if (trans == T_SPRINT)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = RUN + 1;
	}

	void OnExit() override
	{}

	void Update(Transition& transID) override
	{
		glm::vec3 fwd = body->GetTransform().R[2];
		body->SetVelocity(RUN_SPEED * fwd);
	}
};