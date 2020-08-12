#pragma once

#include "AnimState.h"

#define AIR_TIME 1.667f
#define deltaT  (1.0f / 60.0f)

class IdleJumpState : public AnimState
{
public:
	IdleJumpState(Body* body)
	: AnimState(body), elapsed(0.0f), airborne(false) {}

	bool Trigger(Transition& trans) override
	{
		if (trans == T_IDLE)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = IDLE_JUMP + 1;
		elapsed = 0.0f;
		airborne = false;
	}

	void OnExit() override
	{}

	void Update(Transition& transID) override
	{
		elapsed += deltaT;
		if (elapsed >= 0.5f && !airborne)
		{
			airborne = true;
			glm::vec3 v = body->GetVelocity();
			v.y += 2.5f;
			body->SetVelocity(v);
		}
		if (elapsed > AIR_TIME)
		{
			transID = T_IDLE;
		}
	}

private:
	float elapsed;
	bool airborne;
};