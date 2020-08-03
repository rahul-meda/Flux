#pragma once

#include <vector>
#include "AnimState.h"

class AnimFSM
{
public:
	AnimFSM();
	void Update();
	Transition transition;
	Body* body;
	AnimState* state;
	unsigned int animID;
	std::vector<AnimState*> states;
};