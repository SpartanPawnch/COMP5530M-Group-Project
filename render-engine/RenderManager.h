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
    void setupCubemapPipelineUniforms();
    void setupIconPipelineUniforms();
    void loadIcons();
    void runPipeline(Pipeline pipeline);

    static void uploadMesh(std::vector<Vertex>* v, std::vector<unsigned int>* i, unsigned int* VAO,
        unsigned int* VBO, unsigned int* EBO);
    static void deleteMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO);
    void setGammaCorrection(float value);
};


#include <PxPhysics.h>
#include <PxPhysicsAPI.h>
#include "../../src/logging.h"

enum class FilterGroup : physx::PxU32
{
    //this might be needed.
    /*eSTATIC = (1 << 0),
    eKINEMATIC = (1 << 1)*/
    //add custom logic.
    //else
    eSPHERE = (1 << 0),  // Add any class we 
    eCUBE = (1 << 1),  // Group 2
    eWALL = (1 << 2),  // Group 3
    eDESTROYABLE = (1 << 3), // Group 4
    eEXPLOSION = (1 << 4)
};

//static objects needed for physX
static bool EnablePhysics = true;
static bool EnableDebugging = true;


class physXSingleTon
{
private:
    //this is the main physics engine. 
    //physx varibles copied
    //init variables.
    physx::PxFoundation* mFoundation = nullptr;
    physx::PxPhysics* mPhysics = nullptr;
    physx::PxDefaultCpuDispatcher* mDispatcher = nullptr;
    physx::PxScene* mScene = nullptr;
    physx::PxMaterial* mMaterial = nullptr;

    physx::PxPvd* mPvd = nullptr;
    physx::PxPvdTransport* mTransport = nullptr;

    //host and port
    //refered from hello world.
    const std::string mHost = "127.0.0.1";
    const unsigned int mPort = 5425;
    unsigned int mTimeout = 10;

    //error callback
    physx::PxDefaultAllocator mAllocator;

    //scene elements
    bool mInitialised;

    //error list

    //rigid bodies
    std::vector<physx::PxRigidDynamic*> rigidBodies;
    std::vector<physx::PxShape*> rigidBodyShapes;

    //create physics cooking
    physx::PxCooking* mCooking;

    bool mCudaSupport = false;
    bool mOutstandingAllocation = false;


    //


    static physXSingleTon* instance;
    physXSingleTon();
    physXSingleTon(const physXSingleTon&) = delete;
    physXSingleTon& operator=(const physXSingleTon&) = delete;
public:
    static physXSingleTon* getInstance();
    void createSceneDescriptor(physx::PxSceneDesc& sceneDesc);
    physx::PxPhysics* createPhysics();
    void createMeshDescriptor(physx::PxTriangleMeshDesc& meshDesc, Model* model);

    //create rigid objects here.
    std::vector<physx::PxRigidDynamic*> rigidBodiesWall;
    std::vector<physx::PxRigidDynamic*> rigidBodiesSphere;
    std::vector<physx::PxRigidDynamic*> rigidBodiesCube;

    //create sphere.
    physx::PxRigidDynamic* createSphere(physx::PxVec3 pos, physx::PxShape* shape, float radius, float density);
    physx::PxRigidDynamic* createCube(physx::PxVec3 pos, physx::PxShape* shape, float length, float density);
    physx::PxTransform getPositionAndRotation(physx::PxVec3& pos, physx::PxVec3& rotation, physx::PxRigidDynamic actor);

};



//filter for collision callback
//Since we only need a minimilistic collision filter, this is refered from the collison filter documentation.
//https://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Manual/RigidBodyCollision.html
physx::PxFilterFlags CollisionFilterShader(
    physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);



//link to the logging function.
class physXErrorCallback : public physx::PxErrorCallback
{
    void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
};


//refered from 
class CollisionCallbacks : public physx::PxSimulationEventCallback
{
public:
    void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
    void onWake(physx::PxActor** actors, physx::PxU32 count);
    void onSleep(physx::PxActor** actors, physx::PxU32 count);
    //this is the function we are concerned with:
    //https://gameworksdocs.nvidia.com/PhysX/4.1/documentation/physxguide/Manual/AdvancedCollisionDetection.html#extracting-contact-information
    void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
    void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
    void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);
};


//helper class to load vertices from models
class PhysicsModel
{
private:
    std::vector<physx::PxVec3> vertices;
    std::vector<physx::PxVec3> normals;
    std::vector<physx::PxU32> indices;
    PhysicsModel(Model& model);
    Model* mModel = nullptr;
    bool mInitialised;
public:
    //return the size of the vertices for the creation of meshDesc
    physx::PxU32 getVertexSize() { return static_cast<physx::PxU32>(vertices.size()); }
    //constructor
    bool isInitialised() const { return mInitialised; };
    Model* getMOdel() { return mModel; }
    //add code for parsing vertices, normals and textures.
    void parseVertices();

};




