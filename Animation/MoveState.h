#pragma once

#include "AnimState.h"

#define WALK_SPEED   2.0f
#define RUN_SPEED    10.0f
#define SPRINT_SPEED 20.0f

class MoveState : public AnimState
{
public:
	MoveState()
	{}

	bool Trigger(Transition trans) override
	{
		if (trans == T_IDLE)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = WALK;
	}

	void OnExit() override
	{}

	void Update(Body* body) override
	{
		glm::vec3 fwd = body->GetTransform().R[2];
		body->SetVelocity(WALK_SPEED * fwd);
	}

private:
	bool alwaysRun;
	bool sprint;
};