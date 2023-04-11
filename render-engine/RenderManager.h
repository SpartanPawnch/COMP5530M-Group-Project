#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "RenderPipeline.h"
#include "Camera.h"
#include "Buffer.h"

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

	//Available Render Pipelines
	std::vector <RenderPipeline> pipelines; //Pipeline refers to a shader program

	//considering windows
	//std::vector <GLFWwindow*> windows;

	//default constructor does nothing
	RenderManager() {}

	// no copy constructor or copy operator
	RenderManager(const RenderManager&) = delete;
	RenderManager& operator=(const RenderManager&) = delete;

	//default destructor does nothing
	~RenderManager() {}

public:

	//members
	//std::vector<RenderPipeline> programs;
	Camera* camera;
	Camera* previewCamera;

	double deltaTime;

	//mouse input variables
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

	void addCamera();

	void addPipeline(const char* vertexPath,
		const char* fragmentPath,
		const char* geometryPath = nullptr,
		const char* computePath = nullptr,
		const char* tessControlPath = nullptr,
		const char* tessEvalPath = nullptr);

	//void AddWindow(int width, int height, const char* windowTitle);

	// To create the Pipeline-Entity Map
	void addEntityToPipeline();

	void renderScene(Camera camera, GLFWwindow* window);

	void updateMatrices(int* width, int* height);

	void shutDown();

	RenderPipeline* getPipeline(std::size_t index) 
	{
		return &this->pipelines[index];
	}

};