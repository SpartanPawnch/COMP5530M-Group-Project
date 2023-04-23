#pragma once
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <iostream>

#include <glm/glm.hpp>

class LightSource
{
private:

	glm::vec3 position;
	glm::vec3 colour;


public:
	LightSource(const glm::vec3& position, const glm::vec3& colour);

	//setters
	void setPosition(const glm::vec3& newPosition);

	void setColour(const glm::vec3& newColour);

	//getters
	const glm::vec3& getPosition() const;

	const glm::vec3& getColour() const;

};

#endif // CAMERA_H