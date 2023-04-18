#include "Camera.h"

void Camera::updateCameraProperties()
{
    this->front.x = cos(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));
    this->front.y = sin(glm::radians(this->pitch));
    this->front.z = sin(glm::radians(this->yaw)) * cos(glm::radians(this->pitch));

    this->front = glm::normalize(this->front);
    this->right = glm::normalize(glm::cross(this->front, this->worldUp));
    this->up = glm::normalize(glm::cross(this->right, this->front));
}

Camera::Camera(glm::vec3 position, glm::vec3 direction)
{
    this->viewMatrix = glm::mat4(1.f);

    this->defaultSpeed = 1.0f;
    this->highSpeed = 10.0f;
    this->lowSpeed = 0.3f;
    this->movementSpeed = this->defaultSpeed;
    this->sensitivity = 5.0f;

    this->worldUp = glm::vec3(0.f, 1.f, 0.f);
    this->position = position;
    this->initialPosition = position;
    this->right = glm::vec3(0.f);
    this->up = glm::vec3(0.f, 1.f, 0.f);
    this->focusState = false;

    this->pitch = 0.f;
    this->yaw = -90.f;
    this->roll = 0.f;

    this->updateCameraProperties();
}

const glm::mat4 Camera::getViewMatrix()
{
    this->updateCameraProperties();

    this->viewMatrix = glm::lookAt(this->position, this->position + this->front, this->up);

    return this->viewMatrix;
}

const glm::vec3 Camera::getPosition()
{
    return this->position;
}

void Camera::resetPosition()
{
    this->position = this->initialPosition;
}

void Camera::updateKeyboardInput(const float& deltaTime, const int direction)
{
    //Update position vector
    switch (direction) {
    case FORWARD:
        this->position += this->front * this->movementSpeed * deltaTime;
        break;
    case BACKWARD:
        this->position -= this->front * this->movementSpeed * deltaTime;
        break;
    case LEFT:
        this->position -= this->right * this->movementSpeed * deltaTime;
        break;
    case RIGHT:
        this->position += this->right * this->movementSpeed * deltaTime;
        break;
    case ASCEND:
        this->position += glm::vec3(0.0f, 1.0f, 0.0f) * this->movementSpeed * deltaTime;
        break;
    case DESCEND:
        this->position -= glm::vec3(0.0f, 1.0f, 0.0f) * this->movementSpeed * deltaTime;
        break;
    case FAST:
        if (this->movementSpeed < this->highSpeed)
        {
            this->movementSpeed = this->highSpeed;
        }
        else
        {
            this->movementSpeed = this->defaultSpeed;
        }
        break;
    case SLOW:
        if (this->movementSpeed > this->lowSpeed)
        {
            this->movementSpeed = this->lowSpeed;
        }
        else
        {
            this->movementSpeed = this->defaultSpeed;
        }
    default:
        break;

    }
}

void Camera::updateMouseInput(const float& deltaTime, const double& offsetX, const double& offsetY)
{
    //Update pitch, yaw and roll
    this->pitch += static_cast<GLfloat>(-offsetY) * this->sensitivity * deltaTime;
    this->yaw += static_cast<GLfloat>(offsetX) * this->sensitivity * deltaTime;

    if (this->pitch >= 80.f) {
        this->pitch = 80.f;
    }
    else if (this->pitch <= -80.f) {
        this->pitch = -80.f;
    }

    if (this->yaw > 360.f || this->yaw < -360.f) {
        this->yaw = 0.f;
    }
}

void Camera::updateInput(const float& deltaTime, const int direction, const double& offsetX, const double& offsetY)
{
    this->updateKeyboardInput(deltaTime, direction);
    this->updateMouseInput(deltaTime, offsetX, offsetY);
}