#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#include <iostream>
#include <stdio.h>

#include <cassert>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <thread>
#include <mutex>

#include "fdutil.h"
#include "logging.h"
#include "levels.h"
#include "scripting.h"
#include "metrics.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "model_import/model.h"
#include "../render-engine/RenderManager.h"
#include "ECS/System/InputSystem.h"
#include "hud.h"

// set renderEngine instance to nullptr initially
RenderManager* renderManager;
MaterialSystem* materialSystem;
PhysicsEngine* physicsEngine;
InputSystem* inputSystem;

RenderManager* RenderManager::instance = nullptr;
MaterialSystem* MaterialSystem::instance = nullptr;
PhysicsEngine* PhysicsEngine::instance = nullptr;
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

    metrics::MetricsTracker metricsTracker;

    // init scripting
    scripting::ScriptManager scriptMgr;

    // init logging
    logging::LogManager logMgr;

    // Material System
    materialSystem = MaterialSystem::getInstance();
    materialSystem->createMaterial("test");

    // Render Engine
    renderManager = RenderManager::getInstance();

    physicsEngine = PhysicsEngine::getInstance();
    physicsEngine->createWorld();

    renderManager->startUp(window);

    renderManager->loadScene();

    // Input System
    inputSystem = InputSystem::getInstance();

    // float lastQueryTime = float(glfwGetTime());
    float currTime = float(glfwGetTime());

    // init textures
    TextureManager texManager;

    // audio controls
    audio::AudioEngine audioEngine;

    // create scene
    Scene scene;

    // load project file
    {
        std::string level = loadProjectFile("project.json");
        assert(!level.empty());

        // load level
        loadLevel(level.c_str(), scene);
    }

    scene.start();
    inputSystem->scene = &scene;
    inputSystem->start(window);

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
    ComponentStorage::registerMetatables();
    InputSystem::registerLuaFunctions();

    const float timeStep = 1.0f / 60.0f;
    long double accumulator = 0;

    physicsEngine->isSimulating = true;

    bool lastf12 = false;

    while (!glfwWindowShouldClose(window)) {
        currTime = float(glfwGetTime());
        if (glfwGetKey(window, GLFW_KEY_F12)) {
            if (!lastf12)
                scene.showColliders = !scene.showColliders;

            lastf12 = true;
        }
        else {
            lastf12 = false;
        }

        scene.processQueues();
        updateLevels(scene);

        // get window dimensions
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        double current_time = glfwGetTime();
        renderManager->deltaTime = current_time - previous_time;

        if (physicsEngine->isSimulating) {

            // Add the time difference in the accumulator
            accumulator += current_time - previous_time;

            // While there is enough accumulated time to take
            // one or several physics steps
            while (accumulator >= timeStep) {

                // Update the Dynamics world with a constant time step
                physicsEngine->world->update(timeStep);
                // Decrease the accumulated time
                accumulator -= timeStep;
            }

            renderManager->movePhysicsEntities(scene, &renderManager->camera, width, height);
        }

        // handle inputs
        previous_time = current_time;

        if (scene.showColliders)
            prepUI(width, height);

        // update matrices
        // renderManager->updateMatrices(&width, &height);

        // update scene state
        // scene.updatePositions();
        scene.update(renderManager->deltaTime);

        //--- Draw Results ---
        glViewport(0, 0, width, height);

        // draw scene
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderManager->renderSkybox(scene, &renderManager->camera, width, height);

        renderManager->renderEntities(scene, &renderManager->camera, width, height);

        if (scene.showColliders) {
            renderManager->renderColliders(scene, width, height);
            drawUI();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    return 0;
}
