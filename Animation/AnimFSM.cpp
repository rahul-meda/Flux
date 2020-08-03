
#include "AnimFSM.h"
#include "../Components/Body.h"
#include "IdleState.h"
#include "MoveState.h"

AnimFSM::AnimFSM()
: state(nullptr), animID(unsigned int(IDLE)), transition(T_IDLE), body(nullptr)
{
	states.push_back(new IdleState());
	states.push_back(new MoveState());

	state = states[IDLE];
}

void AnimFSM::Update()
{
	if (state->Trigger(transition))
	{
		state->OnExit();
		state = states[transition];
		state->OnEnter(animID);
	}

	state->Update(body);
}
