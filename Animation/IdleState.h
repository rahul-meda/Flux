#pragma once

#include "AnimState.h"

class IdleState : public AnimState
{
public:
	IdleState()
	{}

	bool Trigger(Transition trans) override
	{
		if (trans == T_MOVE      ||
			trans == T_IDLE_JUMP ||
			trans == T_DANCE     ||
			trans == T_ATTACK    ||
			trans == T_DODGE)
			return true;

		return false;
	}

	void OnEnter(unsigned int& animID) override
	{
		animID = IDLE;
	}

	void OnExit() override
	{}

	void Update(Body* body) override
	{}
};