#include "ai.h"



StateMachine::StateMachine() 
{
	
};

StateMachine::StateMachine(State* state) 
{
	mCurrentState = new baseState();
}

void StateMachine::setState(State* state)
{
	//initialisation crash
	if (mCurrentState)
		mCurrentState->exitState();
	mCurrentState = std::move(state);
	mCurrentState->enterState();
}

void StateMachine::updateMachine(RigidBody* rigidBody)
{
	mCurrentState->updateState(rigidBody);
}



void RandomWalk::exitState()
{
	logging::logInfo("exit randomWalk");
}
void RandomWalk::updateState(RigidBody* rigidBody)
{
	performRandomMovement(rigidBody);
}
void RandomWalk::enterState()
{
	logging::logInfo("entered randomWalk");
}


void Patrol::exitState()
{
	logging::logInfo("exit Patrol");
}
void Patrol::updateState(RigidBody* rigidBody)
{
	RigidBody* dst;
	findClosestRigidBody(dst, 10.0f, rigidBody);
	moveTowardsABodyForce(rigidBody, dst);
}
void Patrol::enterState()
{
	logging::logInfo("entered Patrol");
}

void Sleep::exitState()
{
	logging::logInfo("exit Patrol");
}
void Sleep::updateState(RigidBody* rigidBody)
{
	rigidBody->setLinearVelocity(Vector3(0.0f, 0.0f, 0.0f));
	rigidBody->enableGravity(0);
	rigidBody->setAngularVelocity(Vector3(0.0f, 0.0f, 0.0f));
}
void Sleep::enterState()
{
	logging::logInfo("entered Sleep");
}


baseState::baseState() :
	stateName("base") {};

void baseState::exitState()
{
	logging::logInfo("exit baseState");
}
void baseState::updateState(RigidBody* rigidBody)
{
	
}
void baseState::enterState()
{
	logging::logInfo("entered baseState");
	stateName = "base";
}



Sleep::Sleep()
{
	stateName = "Sleep";
}

Patrol::Patrol()
{
	stateName = "Patrol";
}

RandomWalk::RandomWalk()
{
	stateName = "Random";
}