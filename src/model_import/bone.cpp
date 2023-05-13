#include "bone.h"

void Bone::getPositionPrevAndNext(float animationTime, KeyPosition& prev, KeyPosition& next) {
    for (uint32_t i = 1u; i < positionKeys.size(); i++) {
        if (animationTime < positionKeys[i].time) {
            prev = positionKeys[i - 1];
            next = positionKeys[i];
            return;
        }
    }
    assert(0);
}

void Bone::getRotationPrevAndNext(float animationTime, KeyRotation& prev, KeyRotation& next) {
    for (uint32_t i = 1u; i < rotationKeys.size(); i++) {
        if (animationTime < rotationKeys[i].time) {
            prev = rotationKeys[i - 1];
            next = rotationKeys[i];
            return;
        }
    }
    assert(0);
}

void Bone::getScalePrevAndNext(float animationTime, KeyScale& prev, KeyScale& next) {
    for (uint32_t i = 1u; i < scaleKeys.size(); i++) {
        if (animationTime < scaleKeys[i].time) {
            prev = scaleKeys[i - 1];
            next = scaleKeys[i];
            return;
        }
    }
    assert(0);
}

float Bone::getInterpolationFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    float timeBetweenFrames = nextTimeStamp - lastTimeStamp;
    float timeMissingForNext = animationTime - lastTimeStamp;
    return timeMissingForNext / timeBetweenFrames;
}

glm::mat4 Bone::interpolatePosition(float animationTime) {
    if (positionKeys.size() == 0) {
        return glm::mat4(1.0f);
    }
    if (positionKeys.size() == 1) {
        return glm::translate(glm::mat4(1.0f), positionKeys.at(0).position);
    }
    KeyPosition prev, next;
    getPositionPrevAndNext(animationTime, prev, next);
    float interpolationFactor = getInterpolationFactor(prev.time, next.time, animationTime);
    glm::vec3 keyPosition = glm::mix(prev.position, next.position, interpolationFactor);

    /*std::cout << "PositionPrev: " << std::endl;
    std::cout << glm::to_string(prev.position) << std::endl;
    std::cout << "PositionPref------------" << std::endl;
    std::cout << "PositionAft: " << std::endl;
    std::cout << glm::to_string(next.position) << std::endl;
    std::cout << "PositionAft------------" << std::endl;*/
    return glm::translate(glm::mat4(1.0f), keyPosition);
}

glm::mat4 Bone::interpolateRotation(float animationTime) {
    if (rotationKeys.size() == 0) {
        return glm::mat4(1.0f);
    }
    if (rotationKeys.size() == 1) {
        auto rotation = glm::normalize(rotationKeys.at(0).rotation);
        return glm::toMat4(rotation);
    }

    KeyRotation prev, next;
    getRotationPrevAndNext(animationTime, prev, next);
    float interpolationFactor = getInterpolationFactor(prev.time, next.time, animationTime);
    glm::quat keyRotation = glm::slerp(prev.rotation, next.rotation, interpolationFactor);
    keyRotation = glm::normalize(keyRotation);
    return glm::toMat4(keyRotation);
}

glm::mat4 Bone::interpolateScale(float animationTime) {
    if (scaleKeys.size() == 0) {
        return glm::mat4(1.0f);
    }
    if (scaleKeys.size() == 1) {
        return glm::scale(glm::mat4(1.0f), scaleKeys.at(0).scale);
    }

    KeyScale prev, next;
    getScalePrevAndNext(animationTime, prev, next);
    float interpolationFactor = getInterpolationFactor(prev.time, next.time, animationTime);
    glm::vec3 keyScale = glm::mix(prev.scale, next.scale, interpolationFactor);
    return glm::scale(glm::mat4(1.0f), keyScale);
}

glm::vec3 Bone::interpolatePositionDirectly(float animationTime) {
    if (positionKeys.size() == 0) {
        return glm::vec3(0.0f);
    }
    if (positionKeys.size() == 1) {
        return positionKeys.at(0).position;
    }
    KeyPosition prev, next;
    getPositionPrevAndNext(animationTime, prev, next);
    float interpolationFactor = getInterpolationFactor(prev.time, next.time, animationTime);
    return glm::mix(prev.position, next.position, interpolationFactor);
}

glm::quat Bone::interpolateRotationDirectly(float animationTime) {
    if (rotationKeys.size() == 0) {
        return glm::quat();
    }
    if (rotationKeys.size() == 1) {
        glm::quat rotation = glm::normalize(rotationKeys.at(0).rotation);
        return rotation;
    }

    KeyRotation prev, next;
    getRotationPrevAndNext(animationTime, prev, next);
    float interpolationFactor = getInterpolationFactor(prev.time, next.time, animationTime);
    return glm::slerp(prev.rotation, next.rotation, interpolationFactor);
}

glm::vec3 Bone::interpolateScaleDirectly(float animationTime) {
    if (scaleKeys.size() == 0) {
        return glm::vec3(0.0f);
    }
    if (scaleKeys.size() == 1) {
        return scaleKeys.at(0).scale;
    }

    KeyScale prev, next;
    getScalePrevAndNext(animationTime, prev, next);
    float interpolationFactor = getInterpolationFactor(prev.time, next.time, animationTime);
    return glm::mix(prev.scale, next.scale, interpolationFactor);
}
