
#pragma once

#include <vector>
#include "../Components/Body.h"

enum Transition
{
	T_IDLE = 0,
	T_MOVE,
	T_IDLE_JUMP,
	T_DANCE,
	T_ATTACK,
	T_DODGE
};

enum AnimID
{
	IDLE = 0,
	WALK,
	RUN,
	SPRINT,
	IDLE_JUMP,
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