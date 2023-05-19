#pragma once
//i have used https://www.aleksandrhovhannisyan.com/blog/implementing-a-finite-state-machine-in-cpp/ as a reference

#include "../logging.h"
#include "../physics_engine/physicsEngine.h"
#include "../ECS/EntityState/EntityState.h"

// refered from https://www.aleksandrhovhannisyan.com/blog/implementing-a-finite-state-machine-in-cpp/

class StateMachine;

//mainly for debugging and logging
enum class AI_STATES
{
	RANDOM = 0,
	SLEEP = 1,
	PATROL = 2,
	CHASE = 3,
	BASE = 4
};

class State
{
protected:
	StateMachine* mStateMachine = nullptr;
public:
	State() :mStateMachine(nullptr) {};
	State(StateMachine* sm);
	virtual void exitState() = 0;
	virtual void updateState() = 0; //this function contains the bulk of our code.
	virtual void enterState() = 0;
	//not really needed
};

class StateMachine
{
protected:
	State* mCurrentState = nullptr;
public:
	StateMachine();
	StateMachine(State* state);
	void setState(State* state);
	State* getState() { return mCurrentState; }
	//switch to next state;
	void updateMachine();
};

//define some commonly used states;
class RandomWalk : public State
{
protected:
	StateMachine* mStateMachine = nullptr;
public:
	RandomWalk() :mStateMachine(nullptr) {};
	RandomWalk(StateMachine* sm);
	void exitState();
	void updateState();
	void enterState();
};


class Patrol : public State
{
protected:
	StateMachine* mStateMachine = nullptr;
public:
	Patrol(StateMachine* sm);
	void exitState();
	void updateState();
	void enterState();
};

class Sleep : public State
{
protected:
	StateMachine* mStateMachine = nullptr;
public:
	Sleep(StateMachine* sm);
	void exitState();
	void updateState();
	void enterState();
};

