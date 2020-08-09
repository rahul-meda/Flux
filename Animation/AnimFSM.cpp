
#include "AnimFSM.h"
#include "../Components/Body.h"
#include "states.h"

AnimFSM::AnimFSM()
 : state(nullptr), animID(unsigned int(IDLE)), transition(T_IDLE), triggered(false), body(nullptr)
{}

void AnimFSM::Init(Body* body)
{
	states.push_back(new IdleState(body));
	states.push_back(new WalkState(body));
	states.push_back(new IdleState(body));
	states.push_back(new IdleState(body));
	states.push_back(new RunState(body));
	states.push_back(new IdleState(body));
	states.push_back(new IdleState(body));
	states.push_back(new SprintState(body));
	states.push_back(new IdleJumpState(body));

	state = states[IDLE];
}

bool AnimFSM::Update()
{
	triggered = false;

	if (state->Trigger(transition))
	{
		triggered = true;
		state->OnExit();
		state = states[transition];
		state->OnEnter(animID);
	}

	state->Update(transition);

	return triggered;
}
