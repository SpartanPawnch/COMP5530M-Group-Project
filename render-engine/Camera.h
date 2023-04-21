#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>

#include <GL/glew.h>

#include <glm/ext.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum direction {
    FORWARD = 0,
    BACKWARD,
    LEFT,
    RIGHT,
    ASCEND,
    DESCEND,
    FAST,
    SLOW
};

class Camera {

  private:
    // Variables
    glm::mat4 viewMatrix;

    GLfloat movementSpeed;
    GLfloat defaultSpeed;
    GLfloat lowSpeed;
    GLfloat highSpeed;

    GLfloat sensitivity;

    glm::vec3 worldUp;
    glm::vec3 initialPosition;
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 right;
    glm::vec3 up;

    GLfloat pitch;
    GLfloat yaw;
    GLfloat roll;

    void updateCameraProperties();

  public:
    bool focusState;
    float fov = 45.0f;

    Camera(glm::vec3 position, glm::vec3 direction);

    ~Camera() {
    }

    // Getters
    const glm::mat4 getViewMatrix();

    const glm::vec3 getPosition();

    void resetPosition();

    // Functions
    void updateKeyboardInput(const float& deltaTime, const int direction);
    void updateMouseInput(const float& deltaTime, const double& offsetX, const double& offsetY);

    void updateInput(
        const float& deltaTime, const int direction, const double& offsetX, const double& offsetY);

    void setDirect(const glm::mat4& _view, const float _fov);
};

#endif // CAMERA_H
