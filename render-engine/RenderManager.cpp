#include "RenderManager.h"

RenderEngine* RenderEngine::getInstance()
{
	if (instance == nullptr)
	{
		instance = new RenderEngine();
		std::cout << "Render Instance created now." << std::endl;
				
	}
	else
	{
		std::cout << "Render instance has been created previously. Returning previous instance." << std::endl;
		
	}
	return instance;
}