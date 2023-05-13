// adapted from https://learnopengl.com/Model-Loading/Mesh
// and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

struct KeyPosition {
    glm::vec3 position = glm::vec3(0.0f);
    float time = 0;
};

struct KeyRotation {
    glm::quat rotation = glm::quat(1.f, .0f, .0f, .0f);
    float time = 0;
};

struct KeyScale {
    glm::vec3 scale = glm::vec3(1.0f);
    float time = 0;
};

class Bone {
  public:
    int id = -1;
    std::string name;
    glm::mat4 transform = glm::mat4(1.f);
    std::vector<Bone> children;

    // keyframes
    std::vector<KeyPosition> positionKeys;
    std::vector<KeyRotation> rotationKeys;
    std::vector<KeyScale> scaleKeys;
    int positionKeysCount;
    int rotationKeysCount;
    int scaleKeysCount;

    void getPositionPrevAndNext(float animationTime, KeyPosition& prev, KeyPosition& next);
    void getRotationPrevAndNext(float animationTime, KeyRotation& prev, KeyRotation& next);
    void getScalePrevAndNext(float animationTime, KeyScale& prev, KeyScale& next);
    float getInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 interpolatePosition(float animationTime);
    glm::mat4 interpolateRotation(float animationTime);
    glm::mat4 interpolateScale(float animationTime);
    glm::vec3 interpolatePositionDirectly(float animationTime);
    glm::quat interpolateRotationDirectly(float animationTime);
    glm::vec3 interpolateScaleDirectly(float animationTime);
};
