#pragma once

#include "AnimState.h"

#define WALK_SPEED 2.0f

class WalkState : public AnimState
{
public:
	WalkState(Body* body)
	: AnimState(body) {}

	bool Trigger(Transition& trans) override
	{
		if (trans == T_IDLE)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = WALK + 1;
	}

	void OnExit() override
	{}

	void Update(Transition& transID) override
	{
		glm::vec3 fwd = body->GetTransform().R[2];
		body->SetVelocity(WALK_SPEED * fwd);
	}
};