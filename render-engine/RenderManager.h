#pragma once

#include <iostream>
#include <fstream>
#include <vector>

#include "RenderPipeline.h"
#include "Camera.h"
#include "Buffer.h"

#include <GLFW/glfw3.h>

#include <glm/ext.hpp>

enum Pipeline {
    ColorPipeline = 0,
    TexturePipeline = 1,
    ShadowPipeline = 2,
    BillboardPipeline = 3,
    WaterPipeline = 4,
    Render2DPipeline = 5
};

class RenderManager {
  private:
    // pointer to single instance of RenderEngine
    static RenderManager* instance;
    bool initialized = false;

    // test output writing for RenderEngine
    // std::ofstream testOutput;

    // Available Render Pipelines
    // The Index of Pipeline Should match the Enum
    std::vector<RenderPipeline> pipelines; // Pipeline refers to a shader program

    // considering windows
    // std::vector <GLFWwindow*> windows;

    // default constructor does nothing
    RenderManager() {
    }

    // no copy constructor or copy operator
    RenderManager(const RenderManager&) = delete;
    RenderManager& operator=(const RenderManager&) = delete;

    // default destructor does nothing
    ~RenderManager() {
    }

    ///
    void runColorPipeline();
    void runTexturePipeline();
    void runShadowPipeline();
    void runBillboardPipeline();
    void runWaterPipeline();
    void run2DPipeline();

  public:
    // members
    // std::vector<RenderPipeline> programs;
    // TODO
    Camera* camera;
    Camera* previewCamera;

    double deltaTime;

    // mouse input variables
    double xPos, yPos, xPosLast, yPosLast;
    double xOffset, yOffset;
    bool firstRClick;

    // matrices
    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;

    // needs to be static to be invoked without object of class
    static RenderManager* getInstance();

    void startUp(GLFWwindow* aWindow);

    void addCamera();

    // TODO: Should probably be called in the Constructor - Now in loadScene()
    void addPipeline(const char* vertexPath, const char* fragmentPath,
        const char* geometryPath = nullptr, const char* computePath = nullptr,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr);

    // void AddWindow(int width, int height, const char* windowTitle);

    // To create the Pipeline-Entity Map
    void addEntityToPipeline();

    // Load Models, Cameras, Lights, Shaders etc to RenderEngine
    void loadScene();
    void renderScene(Camera* camera, GLFWwindow* window);
    void renderSceneRefactor(Camera* camera, int width, int height);

    void updateMatrices(int* width, int* height);

    void shutDown();

    RenderPipeline* getPipeline(std::size_t index) {
        return &this->pipelines[index];
    }

    void runPipeline(Pipeline pipeline);
};
