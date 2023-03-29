#pragma once
#include<string>
#include<thread>
#include<GL/glew.h>
#include<GLFW/glfw3.h>

struct GUIManager {
    GUIManager();
};

extern std::string activePath;

extern std::thread projectThread;

//define UI structure
void prepUI(GLFWwindow* window, const char* executablePath, float dt,
    int viewportWidth, int viewportHeight);

//send draw command to GPU
void drawUI();