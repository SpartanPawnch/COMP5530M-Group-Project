#include "ai.h"

StateMachine::StateMachine() :
	mCurrentState(nullptr)
{
};

StateMachine::StateMachine(State* state) :
	mCurrentState(state)
{
}

void StateMachine::setState(State* state)
{
	//initialisation crash
	if (mCurrentState)
		mCurrentState->exitState();
	mCurrentState = state;
	mCurrentState->enterState();
}

void StateMachine::updateMachine()
{
	mCurrentState->updateState();
}

State::State(StateMachine* sm) :
	mStateMachine(sm) {};

RandomWalk::RandomWalk(StateMachine* sm) :
	mStateMachine(sm) {};

Patrol::Patrol(StateMachine* sm) :
	mStateMachine(sm) {};

Sleep::Sleep(StateMachine* sm) :
	mStateMachine(sm) {};

void RandomWalk::exitState()
{
	logging::logInfo("%s ", "exit randomWalk");
}
void RandomWalk::updateState()
{
	logging::logInfo("%s ", "update randomWalk");
}
void RandomWalk::enterState()
{
	logging::logInfo("%s ", "entered randomWalk");
}


void Patrol::exitState()
{
	logging::logInfo("%s ", "exit Patrol");
}
void Patrol::updateState()
{
	logging::logInfo("%s ", "update Patrol");
}
void Patrol::enterState()
{
	logging::logInfo("%s ", "entered Patrol");
}

void Sleep::exitState()
{
	logging::logInfo("%s ", "exit Patrol");
}
void Sleep::updateState()
{
	logging::logInfo("%s ", "update Patrol");
}
void Sleep::enterState()
{
	logging::logInfo("%s ", "entered Patrol");
}
