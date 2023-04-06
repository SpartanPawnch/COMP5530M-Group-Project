#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "ShaderProgram.h"
#include "Camera.h"
#include "RenderPipeline.h"


#include <GLFW/glfw3.h>

#include <glm/ext.hpp>


class RenderManager
{
private:
	
	//pointer to single instance of RenderEngine
	static RenderManager* instance;
	bool initialized = false;

	//Available Render Pipelines
	std::vector <RenderPipeline> pipelines; //Pipeline refers to a shader program
	std::vector <Camera> cameras;
	std::vector <GLFWwindow*> windows;

	//default constructor does nothing
	RenderManager() {}

	// no copy constructor or copy operator
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

	//default destructor does nothing
	~RenderManager() {}


public:

	//members
	//std::vector<ShaderProgram> programs; // moved to render pipelines
	//Camera* camera;

	double deltaTime;
	double xPos, yPos, xPosLast, yPosLast;
	double xOffset, yOffset;
	bool firstRClick;

	//matrices // moved to Camera
	//glm::mat4 modelMatrix;
	//glm::mat4 projectionMatrix;
	//glm::mat4 viewMatrix;

	//needs to be static to be invoked without object of class
	static RenderManager* getInstance();

	void startUp(GLFWwindow* aWindow);

	//void addProgram(ShaderProgram aProgram);


	void shutDown();

	//When camera is added to scene, initialize and add it to the class vector
	void AddCamera();

	void AddPipeline();

	// To create the Pipeline-Entity Map
	void AddToPipeline();


};