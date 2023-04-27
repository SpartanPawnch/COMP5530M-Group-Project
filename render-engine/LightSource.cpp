#include "LightSource.h"

LightSource::LightSource(const glm::vec3& position, const glm::vec3& ambient,
	const glm::vec3& diffuse, const glm::vec3& specular) :
	position(position), ambient(ambient), diffuse(diffuse), specular(specular)
{}

//setters
void LightSource::setPosition(const glm::vec3& newPosition)
{
	this->position = newPosition;
}

void LightSource::setAmbient(const glm::vec3& newAmbient)
{
	this->ambient = newAmbient;
}

void LightSource::setDiffuse(const glm::vec3& newDiffuse)
{
	this->diffuse = newDiffuse;
}

void LightSource::setSpecular(const glm::vec3& newSpecular)
{
	this->specular = newSpecular;
}

//getters
const glm::vec3& LightSource::getPosition() const
{
	return position;
}

const glm::vec3& LightSource::getAmbient() const
{
	return ambient;
}

const glm::vec3& LightSource::getDiffuse() const
{
	return diffuse;
}

const glm::vec3& LightSource::getSpecular() const
{
	return specular;
}