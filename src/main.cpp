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
#include "metrics.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "asset_import/materials.h"
#include "model_import/model.h"
#include "../render-engine/RenderManager.h"
#include "ECS/System/InputSystem.h"

RenderManager* renderManager;
MaterialSystem* materialSystem;

InputSystem* inputSystem;

// set renderEngine instance to nullptr initially
RenderManager* RenderManager::instance = nullptr;
MaterialSystem* MaterialSystem::instance = nullptr;

// set InputSystem to nullptr intially
InputSystem* InputSystem::instance = nullptr;

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

    // init metrics
    metrics::MetricsTracker metricsTracker;
    // init scripting
    scripting::ScriptManager scriptMgr;

    // init logging
    logging::LogManager logMgr;

    // Probably the job of Asset Manager
    ///////////////////////////////////
    // Load entity models
    // Model model;
    // model.loadModel("assets/tree.obj");

    //////////////////////////////////

    materialSystem = MaterialSystem::getInstance();
    materialSystem->createMaterial("test");

    // Render Engine
    renderManager = RenderManager::getInstance();

    renderManager->startUp(window);

    renderManager->loadScene();
    renderManager->loadIcons();

    // Input System
    inputSystem = InputSystem::getInstance();
    inputSystem->start(window);

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
    renderManager->setupAnimatedPipelineUniforms();
    renderManager->setupEntIDPipelineUniforms();
    renderManager->setupCubemapPipelineUniforms();
    renderManager->setupIconPipelineUniforms();
    renderManager->setupIconIDPipelineUniforms();
    renderManager->setupAnimationIDPipelineUniforms();

    // register lua stuff
    scene.registerLuaTable();

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
        updateLevels(scene);
        //--- Draw Results ---
        // draw scene to texture
        glBindFramebuffer(GL_FRAMEBUFFER, viewportMultisampleFramebuffer);
        glViewport(0, 0, viewportTexWidth, viewportTexHeight);

        GLuint attachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
        glDrawBuffers(2, attachments);

        // clear screen
        {
            const float clearVals[] = {.2f, .2f, .2f, 1.f};
            const float clearDepth = 1.f;
            glClearBufferfv(GL_COLOR, 0, clearVals);
            glClearBufferfv(GL_DEPTH, 0, &clearDepth);
        }

        renderManager->renderSkybox(scene, &renderManager->camera, viewportTexWidth,
            viewportTexHeight);

        // draw grid
        glDisable(GL_CULL_FACE);
        renderManager->renderGrid(&renderManager->camera, viewportTexWidth, viewportTexHeight);
        glEnable(GL_CULL_FACE);

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
            CameraComponent& cam =
                scene.selectedEntity->components.vecCameraComponent[scene.selectedCameraIdx];
            // setup preview camera object
            cam.copyToCamera(renderManager->previewCamera,
                scene.selectedEntity->state.runtimeTransform);

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

        glBindFramebuffer(GL_FRAMEBUFFER, viewportFramebuffer);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, viewportMultisampleFramebuffer);
        glBlitFramebuffer(0, 0, viewportTexWidth, viewportTexHeight, 0, 0, viewportTexWidth,
            viewportTexHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // wait for results
        glBindFramebuffer(GL_FRAMEBUFFER, entIDFramebuffer);
        glViewport(0, 0, viewportTexWidth, viewportTexHeight);

        GLuint entIDattachments[] = {GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT};
        glDrawBuffers(2, entIDattachments);

        // clear screen
        {
            const float clearVals[] = {.0f, .0f, .0f, 1.f};
            const float clearDepth = 1.f;
            glClearBufferfv(GL_COLOR, 0, clearVals);
            glClearBufferfv(GL_DEPTH, 0, &clearDepth);
        }

        renderManager->renderEntitiesID(scene, &renderManager->camera, viewportTexWidth,
            viewportTexHeight);

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
