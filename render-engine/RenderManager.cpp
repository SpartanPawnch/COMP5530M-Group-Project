
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

	this->pipelines.clear();
	this->deltaTime = 0.0f;
	this->xPos = 0.0f;
	this->yPos = 0.0f;
	this->xPosLast = 0.0f;
	this->yPosLast = 0.0f;
	this->firstRClick = true;
	this->xOffset = 0.0f;
	this->yOffset = 0.0f;

	//Initialise the camera
	this->camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 0.f, 0.f));

	this->modelMatrix = glm::mat4(1.0f);
	this->viewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->projectionMatrix = glm::mat4(1.0f);
}

void RenderManager::addPipeline(const char* vertexPath, const char* fragmentPath, const char* geometryPath,
	const char* computePath, const char* tessControlPath, const char* tessEvalPath)
{
	RenderPipeline newPipeline = RenderPipeline(vertexPath, fragmentPath, geometryPath,
		computePath, tessControlPath, tessEvalPath);

	this->pipelines.push_back(newPipeline);
}

void RenderManager::updateMatrices(int* width, int* height)
{
	//Initialize matrices
	this->modelMatrix = glm::mat4(1.0f);
	//this->viewMatrix = glm::lookAt(glm::vec3(2.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	this->viewMatrix = camera->getViewMatrix();
	this->projectionMatrix = glm::perspective(glm::radians(45.0f), (float)*width / (float)*height, 0.01f, 100.0f);
}