#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <memory>

#include "RenderPipeline.h"
#include "Camera.h"
#include "Buffer.h"
#include "../src/ECS/Scene/Scene.h"
#include "LightSource.h"
#include "../src/model_import/model.h"

#include <GLFW/glfw3.h>

#include <glm/ext.hpp>

enum Pipeline {
    ColourPipeline = 0,
    TexturePipeline = 1,
    ShadowPipeline = 2,
    BillboardPipeline = 3,
    WaterPipeline = 4,
    Render2DPipeline = 5,
    GridPipeline = 6,
    FrustumVisPipeline = 7,
    EmptyVisPipeline = 8,
    EntIDPipeline = 9,
    AnimatedPipeline = 10,
    CubemapPipeline = 11,
    IconPipeline = 12,
    IconIDPipeline = 13,
    AnimationIDPipeline = 14,
    Pipeline_MAX
};

struct Buffer {
    VertexBuffer vBuffer;
    IndexBuffer iBuffer;

    Buffer(VertexBuffer v, IndexBuffer i) {
        vBuffer = v;
        iBuffer = i;
    }
};

class RenderManager {
  private:
    // pointer to single instance of RenderEngine
    static RenderManager* instance;
    bool initialized = false;
    bool antialiasingEnabled = false;

    // test output writing for RenderEngine
    // std::ofstream testOutput;

    // Available Render Pipelines
    // The Index of Pipeline Should match the Enum
    std::vector<RenderPipeline> pipelines; // Pipeline refers to a shader program
    std::unordered_map<Pipeline, std::vector<Buffer>> PipelineMeshBufferMap;
    //
    // Light Sources
    std::vector<std::weak_ptr<LightDescriptor>> lightsMetadata;
    std::vector<LightSource> lights;

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

    void runColourPipeline();
    void runTexturePipeline();
    void runShadowPipeline();
    void runBillboardPipeline();
    void runWaterPipeline();
    void run2DPipeline();
    void runGridPipeline();
    void runFrustumVisPipeline();
    void runEmptyVisPipeline();

  public:
    // members
    // TODO
    Camera camera = Camera(glm::vec3(.0f, 2.0f, 8.0f), glm::vec3(.0f, -2.0f, -8.0f));
    Camera previewCamera = Camera(glm::vec3(.0f, .0f, .0f), glm::vec3(.0f, .0f, -5.0f));

    std::shared_ptr<TextureDescriptor> ligthIconDescriptor;
    std::shared_ptr<TextureDescriptor> cameraIconDescriptor;
    std::shared_ptr<TextureDescriptor> soundIconDescriptor;

    double deltaTime;
    // Gamma value for gamma correction
    float gammaValue = 2.2f;

    // mouse input variables
    double xPos, yPos, xPosLast, yPosLast;
    double xOffset, yOffset;
    bool firstRClick;

    // matrices
    glm::mat4 modelMatrix;
    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
    glm::mat4 previewMatrix;

    // needs to be static to be invoked without object of class
    static RenderManager* getInstance();

    void startUp(GLFWwindow* aWindow);

    void shutDown();

    void addCamera();

    std::shared_ptr<LightDescriptor> addLightSource(glm::vec3& position, glm::vec3& ambient,
        glm::vec3& diffuse, glm::vec3& specular);

    void removeLightSource(size_t idx);

    // TODO: Should probably be called in the Constructor - Now in loadScene()
    void addPipeline(Pipeline pipe, const char* vertexPath, const char* fragmentPath,
        const char* geometryPath = nullptr, const char* computePath = nullptr,
        const char* tessControlPath = nullptr, const char* tessEvalPath = nullptr);

    // void AddWindow(int width, int height, const char* windowTitle);

    // To create the Pipeline-Entity Map
    void addMeshToPipeline(std::vector<Pipeline>, VertexBuffer, IndexBuffer, GLuint VAO);

    // Load Models, Cameras, Lights, Shaders etc to RenderEngine
    void loadScene();
    void renderScene(Camera* camera, GLFWwindow* window);
    void renderSceneRefactor(Camera* camera, int width, int height);
    void renderEntities(const Scene& scene, Camera* camera, int width, int height);
    void renderSkybox(const Scene& scene, Camera* camera, int width, int height);
    void renderGrid(Camera* camera, int width, int height);
    void renderEntitiesID(const Scene& scene, Camera* camera, int width, int height);
    void renderCamPreview(const Scene& scene, int width, int height);

    void updateMatrices(int* width, int* height);

    RenderPipeline* getPipeline(Pipeline pipe);

    LightSource* getLightSource(std::size_t index);
    size_t getLightCount() {
        return lights.size();
    }

    void setupColourPipelineUniforms();
    void setupTexturePipelineUniforms();
    void setupAnimatedPipelineUniforms();
    void setupEntIDPipelineUniforms();
    void setupIconIDPipelineUniforms();
    void setupAnimationIDPipelineUniforms();
    void setupCubemapPipelineUniforms();
    void setupIconPipelineUniforms();
    void loadIcons();
    void runPipeline(Pipeline pipeline);

    static void uploadMesh(std::vector<Vertex>* v, std::vector<unsigned int>* i, unsigned int* VAO,
        unsigned int* VBO, unsigned int* EBO);
    static void deleteMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO);
    void setGammaCorrection(float value);
};
