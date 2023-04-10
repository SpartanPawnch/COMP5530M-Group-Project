//adapted from https://learnopengl.com/Model-Loading/Mesh
//and https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation
#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <assimp/scene.h>

struct KeyPosition
{
    glm::vec3 position;
    float time;
};

struct KeyRotation
{
    glm::quat rotation;
    float time;
};

struct KeyScale
{
    glm::vec3 scale;
    float time;
};

class Bone
{
public:
    unsigned int id;
    std::string name;
    glm::mat4 transform;

    //keyframes
    std::vector<KeyPosition> positionKeys;
    std::vector<KeyRotation> rotationKeys;
    std::vector<KeyScale> scaleKeys;

    Bone(const std::string& name, int id, const aiNodeAnim* channel);
    void update(float animationTime);
    int getPositionIndex(float animationTime);
    int getRotationIndex(float animationTime);
    int getScaleIndex(float animationTime);
    float getInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 interpolatePosition(float animationTime);
    glm::mat4 interpolateRotation(float animationTime);
    glm::mat4 interpolateScale(float animationTime);
};