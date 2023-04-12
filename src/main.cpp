#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include <iostream>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <thread>
#include <mutex>

#include "fdutil.h"
#include "drawing.h"
#include "logging.h"
#include "levels.h"
#include "gui.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "model_import/model.h"
#include "../render-engine/RenderManager.h"

// set renderEngine instance to nullptr initially
RenderManager* renderManager;
RenderManager* RenderManager::instance = nullptr;

int main() {
    // switch to correct working directory - platform specific
#ifdef _WIN32
    char executablePath[MAX_PATH];
    int pathLen = GetModuleFileName(NULL, executablePath, MAX_PATH);
    // remove executable name
    for (pathLen--; pathLen >= 0 && executablePath[pathLen] != '\\'; pathLen--) {
        executablePath[pathLen] = '\0';
    }
#else
    char executablePath[PATH_MAX];
    int pathLen = readlink("/proc/self/exe", executablePath, PATH_MAX);
    // remove executable name
    for (pathLen--; pathLen >= 0 && executablePath[pathLen] != '/'; pathLen--) {
        executablePath[pathLen] = '\0';
    }
#endif

    _chdir(executablePath);

    // init glfw and setup window
    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    // create window
    window = glfwCreateWindow(1600, 900, "ONO Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    // setup OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(err) << "\n";
        return 1;
    }

    // swap buffers to avoid the window not responding
    glClearColor(.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    // init scripting
    scripting::ScriptManager scriptMgr;

    // init logging
    logging::LogManager logMgr;

    // Render Engine
    renderManager = RenderManager::getInstance();

    renderManager->startUp(window);

    renderManager->loadScene();
    // init shader

    // float lastQueryTime = float(glfwGetTime());
    float currTime = float(glfwGetTime());

    // init textures
    TextureManager texManager;

    // audio controls
    audio::AudioEngine audioEngine;

    GUIManager guiInstance(window);

    double previous_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        currTime = float(glfwGetTime());
        // get window dimensions
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        double current_time = glfwGetTime();
        renderManager->deltaTime = current_time - previous_time;

        // handle inputs
        previous_time = current_time;

        // update matrices
        // renderManager->updateMatrices(&width, &height);

        // prepare gui
        prepUI(window, executablePath, currTime, width, height);

        //--- Draw Results ---
        // draw scene to texture
        glBindFramebuffer(GL_FRAMEBUFFER, viewportFramebuffer);
        glViewport(0, 0, viewportTexWidth, viewportTexHeight);

        GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
        glDrawBuffers(2, attachments);

        // draw scene
        renderManager->renderSceneRefactor(
            renderManager->camera, viewportTexWidth, viewportTexHeight);

        glFlush();

        // draw UI to full window
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClearColor(.2f, .2f, .2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawUI();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
