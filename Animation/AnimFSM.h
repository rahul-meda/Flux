#pragma once

#include <vector>
#include "AnimState.h"

class AnimFSM
{
public:
	AnimFSM();
	void Init(Body* body);
	bool Update();
	Transition transition;
	bool triggered; // true if a state transition triggered this frame
	Body* body;
	AnimState* state;
	unsigned int animID;
	std::vector<AnimState*> states;
};