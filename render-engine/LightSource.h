#pragma once
#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <iostream>

#include <glm/glm.hpp>

struct LightDescriptor {
    size_t idx = 0;
};

class LightSource {
  private:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

  public:
    LightSource(const glm::vec3& position, const glm::vec3& ambient, const glm::vec3& diffuse,
        const glm::vec3& specular);

    // setters
    void setPosition(const glm::vec3& newPosition);

    void setAmbient(const glm::vec3& newAmbient);

    void setDiffuse(const glm::vec3& newDiffuse);

    void setSpecular(const glm::vec3& newSpecular);

    // getters
    const glm::vec3& getPosition() const;

    const glm::vec3& getAmbient() const;

    const glm::vec3& getDiffuse() const;

    const glm::vec3& getSpecular() const;
};

#endif // CAMERA_H
