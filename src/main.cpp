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
#include "../external/stb_image.h"

#include <string>
#include <thread>
#include <mutex>

#include "fdutil.h"
#include "logging.h"
#include "levels.h"
#include "gui.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "model_import/model.h"
#include "../render-engine/RenderManager.h"

RenderManager* renderManager;

void handleKeyboardInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Move the camera forward
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Move the camera backward
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // Strafe the camera left
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // Strafe the camera right
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 3);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // Ascend camera
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 4);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        // Descend camera
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 5);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        // Reset camera position
        renderManager->camera.resetPosition();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        // increase camera movement speed
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 6);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        // increase camera movement speed
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 7);
    }
}

void handleMouseInput(GLFWwindow* window) {
    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
        if (renderManager->camera.focusState == false) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderManager->camera.focusState = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderManager->camera.focusState = false;
        }

        if (renderManager->firstRClick == true) {
            renderManager->xPosLast = renderManager->xPos;
            renderManager->yPosLast = renderManager->yPos;
            renderManager->firstRClick = false;
        }
    }

    if (renderManager->camera.focusState == true) {
        // now we can change the orientation of the camera
        glfwGetCursorPos(window, &renderManager->xPos, &renderManager->yPos);

        // offset
        renderManager->xOffset = renderManager->xPos - renderManager->xPosLast;
        renderManager->yOffset = renderManager->yPos - renderManager->yPosLast;

        // send data to camera
        renderManager->camera.updateInput(renderManager->deltaTime, -1, renderManager->xOffset,
            renderManager->yOffset);

        renderManager->xPosLast = renderManager->xPos;
        renderManager->yPosLast = renderManager->yPos;
        glfwSetCursorPos(window, renderManager->xPosLast, renderManager->yPosLast);
    }
}
// set renderEngine instance to nullptr initially
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
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // create window
    window = glfwCreateWindow(1600, 900, "ONO Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    // set icon
    {
        GLFWimage icon;
        unsigned char* iconPixels = stbi_load("assets/logo.png", &icon.width, &icon.height, 0, 4);
        assert(iconPixels != nullptr);
        icon.pixels = iconPixels;
        glfwSetWindowIcon(window, 1, &icon);
        stbi_image_free(icon.pixels);
    }
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

    // Probably the job of Asset Manager
    ///////////////////////////////////
    // Load entity models
    Model model;
    model.loadModel("assets/tree.obj");

    //////////////////////////////////

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

    // ------------- UNIFORMS --------------------------
    renderManager->setupColourPipelineUniforms();
    renderManager->setupTexturePipelineUniforms();

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

        // prepare gui and get input
        prepUI(window, executablePath, currTime, width, height);

        // update editor state
        scene.updatePositions();

        //--- Draw Results ---
        // draw scene to texture
        glBindFramebuffer(GL_FRAMEBUFFER, viewportFramebuffer);
        glViewport(0, 0, viewportTexWidth, viewportTexHeight);

        GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
        glDrawBuffers(2, attachments);

        // clear screen
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw grid
        renderManager->renderGrid(&renderManager->camera, viewportTexWidth, viewportTexHeight);

        // draw scene
        renderManager->renderEntities(scene, &renderManager->camera, viewportTexWidth,
            viewportTexHeight);

        // draw cam preview frustum
        renderManager->renderCamPreview(scene, width, height);

        // renderManager->renderSceneRefactor(
        //     renderManager->camera, viewportTexWidth, viewportTexHeight);

        // draw Camera preview window in corner
        if (scene.selectedEntity &&
            scene.selectedEntity->components.vecCameraComponent.size() > 0) {
            CameraComponent& cam = scene.selectedEntity->components.vecCameraComponent[0];
            // setup preview camera object
            renderManager->previewCamera.setDirect(cam.eye, cam.center, cam.up, cam.fov);

            // draw border
            // TODO nicer way to do this
            const int PREVIEW_BORDER = 5;
            const float PREVIEW_FRACTION = .35f; // part of the screen used for preview window
            glEnable(GL_SCISSOR_TEST);
            glScissor((1.f - PREVIEW_FRACTION) * viewportTexWidth - 2 * PREVIEW_BORDER, 0,
                PREVIEW_FRACTION * viewportTexWidth + 2 * PREVIEW_BORDER,
                PREVIEW_FRACTION * viewportTexHeight + 2 * PREVIEW_BORDER);
            glClearColor(.5f, .5f, .5f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // clear preview section
            glScissor((1.f - PREVIEW_FRACTION) * viewportTexWidth - PREVIEW_BORDER, PREVIEW_BORDER,
                PREVIEW_FRACTION * viewportTexWidth, PREVIEW_FRACTION * viewportTexHeight);
            glClearColor(.2f, .2f, .2f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glDisable(GL_SCISSOR_TEST);

            // draw preview
            glViewport((1.f - PREVIEW_FRACTION) * viewportTexWidth - PREVIEW_BORDER, PREVIEW_BORDER,
                PREVIEW_FRACTION * viewportTexWidth, PREVIEW_FRACTION * viewportTexHeight);
            renderManager->renderGrid(&renderManager->previewCamera, viewportTexWidth / 4,
                viewportTexHeight / 4);
            renderManager->renderEntities(scene, &renderManager->previewCamera,
                viewportTexWidth / 4, viewportTexHeight / 4);
        }
        // wait for results
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

    renderManager->shutDown();

    return 0;
}
