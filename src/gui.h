#pragma once
#include<string>
#include<thread>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

struct GUIManager {
    GUIManager(GLFWwindow* window);
    ~GUIManager();
};

extern GLuint viewportFramebuffer;
extern int viewportTexWidth;
extern int viewportTexHeight;

//define UI structure
void prepUI(GLFWwindow* window, const char* executablePath, float dt,
    int viewportWidth, int viewportHeight);

//send draw command to GPU
void drawUI();