#pragma once
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

struct GUIManager {
    GUIManager(GLFWwindow* window);
    ~GUIManager();
};

// define UI structure
void prepUI(int width, int height);

// send draw command to GPU
void drawUI();
