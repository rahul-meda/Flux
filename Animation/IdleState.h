#pragma once

#include "AnimState.h"

class IdleState : public AnimState
{
public:
	IdleState()
	{}

	bool Trigger(Transition trans) override
	{
		if (trans == T_WALK      ||
			trans == T_WALK_LEFT ||
			trans == T_WALK_RIGHT||
			trans == T_RUN		 ||
			trans == T_SPRINT    ||
			trans == T_IDLE_JUMP ||
			trans == T_DANCE     ||
			trans == T_ATTACK    ||
			trans == T_DODGE)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = IDLE + 1;
	}

	void OnExit() override
	{}

	void Update(Body* body) override
	{}
};