
#include "RenderManager.h"

RenderManager* RenderManager::getInstance()
{
	if (instance == nullptr)
	{
		instance = new RenderManager();
		std::cout << "Render Instance created now." << std::endl;
				
	}
	else
	{
		std::cout << "Render instance has been created previously. Returning previous instance." << std::endl;
		
	}
	return instance;
}

void RenderManager::startUp(GLFWwindow* aWindow)
{
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glfwSwapBuffers(aWindow);
	glfwPollEvents();

	this->programs.clear();

	this->modelMatrix = glm::mat4(1.0f);
	this->viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->projectionMatrix = glm::mat4(1.0f);
}

void RenderManager::addProgram(ShaderProgram aProgram)
{
	this->programs.push_back(aProgram);
}

void RenderManager::updateMatrices(int* width, int* height)
{
	//Initialize matrices
	this->modelMatrix = glm::mat4(1.0f);
	this->viewMatrix = glm::lookAt(glm::vec3(2.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->projectionMatrix = glm::perspective(glm::radians(45.0f), (float)*width / (float)*height, 0.01f, 100.0f);
}