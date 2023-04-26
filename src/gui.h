#pragma once
#include <string>
#include <thread>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "ECS/Scene/Scene.h"

struct GUIManager {
    GUIManager(GLFWwindow* window);
    ~GUIManager();
};

extern GLuint viewportMultisampleFramebuffer;
extern GLuint viewportFramebuffer;
extern int viewportTexWidth;
extern int viewportTexHeight;
extern Scene scene;

// define UI structure
void prepUI(GLFWwindow* window, const char* executablePath, float dt, int viewportWidth,
    int viewportHeight);

// send draw command to GPU
void drawUI();
