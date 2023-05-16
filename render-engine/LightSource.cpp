#include "LightSource.h"

LightSource::LightSource(const glm::vec3& position, const glm::vec3& colour) :
	position(position), colour(colour)
{}

//setters
void LightSource::setPosition(const glm::vec3& newPosition)
{
	this->position = newPosition;
}

void LightSource::setColour(const glm::vec3& newColour)
{
	this->colour = newColour;
}

//getters
const glm::vec3& LightSource::getPosition() const
{
	return position;
}

const glm::vec3& LightSource::getColour() const
{
	return colour;
}