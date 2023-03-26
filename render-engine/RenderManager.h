#pragma once

#include <iostream>
#include <fstream>

class RenderEngine
{
private:
	
	//pointer to single instance of RenderEngine
	static RenderEngine* instance;
	bool initialized = false;

	//test output writing for RenderEngine
	//std::ofstream testOutput;

	//default constructor does nothing
	RenderEngine() {}

	// no copy constructor or copy operator
	RenderEngine(const RenderEngine&) = delete;
	RenderEngine& operator=(const RenderEngine&) = delete;

	//default destructor does nothing
	~RenderEngine() {}

public:

	//needs to be static to be invoked without object of class
	static RenderEngine* getInstance();

	void startUp();

	void shutDown();

};