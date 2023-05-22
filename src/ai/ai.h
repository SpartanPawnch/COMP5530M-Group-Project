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

	
public:
	State() {};
	virtual void exitState() = 0;
	virtual void updateState(RigidBody* rigidBody) = 0; //this function contains the bulk of our code.
	virtual void enterState() = 0;
	std::string stateName;
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
	void updateMachine(RigidBody* rigidBody);
};

//define some commonly used states;
class RandomWalk : public State
{
	
public:
	RandomWalk();
	void exitState();
	void updateState(RigidBody* rigidBody);
	void enterState();
	void printState();
	std::string stateName = "Random";
};


class Patrol : public State
{

	
public:
	Patrol();
	void exitState();
	void updateState(RigidBody* rigidBody);
	void enterState();
	std::string stateName = "Patrol";
};

class Sleep : public State
{
	
public:
	Sleep();
	void exitState();
	void updateState(RigidBody* rigidBody);
	void enterState();
	std::string stateName = "Sleep";
};


class baseState : public State
{
	
public:
	baseState();
	void exitState();
	void updateState(RigidBody* rigidBody);
	void enterState();
	std::string stateName = "Base";
};
