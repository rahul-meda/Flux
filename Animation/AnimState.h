
#pragma once

#include <vector>
#include "../Components/Body.h"

enum Transition
{
	T_IDLE = 0,
	T_WALK,
	T_WALK_LEFT,
	T_WALK_RIGHT,
	T_RUN,
	T_RUN_LEFT,
	T_RUN_RIGHT,
	T_SPRINT,
	T_IDLE_JUMP,
	T_MOVING_JUMP,
	T_DANCE,
	T_ATTACK,
	T_DODGE
};

enum AnimID
{
	IDLE = 0,
	WALK,
	WALK_LEFT,
	WALK_RIGHT,
	RUN,
	RUN_LEFT,
	RUN_RIGHT,
	SPRINT,
	IDLE_JUMP,
	MOVING_JUMP,
	DANCE,
	ATTACK,
	DODGE
};

class AnimState
{
public:
	AnimState() {};
	virtual bool Trigger(Transition trans) = 0;
	virtual void OnEnter(unsigned int& animID) = 0;
	virtual void OnExit() = 0;
	virtual void Update(Body* body) = 0;
};