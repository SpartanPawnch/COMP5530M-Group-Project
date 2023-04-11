#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "RenderPipeline.h"
#include "Camera.h"

#include <GLFW/glfw3.h>

#include <glm/ext.hpp>


class RenderManager
{
private:
	
	//pointer to single instance of RenderEngine
	static RenderManager* instance;
	bool initialized = false;

	//test output writing for RenderEngine
	//std::ofstream testOutput;

	//default constructor does nothing
	RenderManager() {}

	// no copy constructor or copy operator
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

	//default destructor does nothing
	~RenderManager() {}

public:

	//members
	std::vector<RenderPipeline> programs;
	Camera* camera;

	double deltaTime;
	double xPos, yPos, xPosLast, yPosLast;
	double xOffset, yOffset;
	bool firstRClick;

	//matrices
	glm::mat4 modelMatrix;
	glm::mat4 projectionMatrix;
	glm::mat4 viewMatrix;

	//needs to be static to be invoked without object of class
	static RenderManager* getInstance();

	void startUp(GLFWwindow* aWindow);

	void addProgram(RenderPipeline aProgram);

	void updateMatrices(int* width, int* height);

	void shutDown();

};