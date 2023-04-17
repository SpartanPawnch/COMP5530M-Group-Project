
#include "RenderManager.h"

RenderManager* RenderManager::getInstance() {
    if (instance == nullptr) {
        instance = new RenderManager();
        std::cout << "Render Instance created now." << std::endl;
    }
    else {
        std::cout << "Render instance has been created previously. Returning previous instance."
                  << std::endl;
    }
    return instance;
}

void RenderManager::startUp(GLFWwindow* aWindow) {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glfwSwapBuffers(aWindow);
    glfwPollEvents();

    this->pipelines.clear();
    this->lights.clear();
    this->deltaTime = 0.0f;
    this->xPos = 0.0f;
    this->yPos = 0.0f;
    this->xPosLast = 0.0f;
    this->yPosLast = 0.0f;
    this->firstRClick = true;
    this->xOffset = 0.0f;
    this->yOffset = 0.0f;

    // Initialise the camera
    this->camera = new Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.f, 0.f, 0.f));

    this->modelMatrix = glm::mat4(1.0f);
    this->viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->projectionMatrix = glm::mat4(1.0f);

}

void RenderManager::shutDown()
{
    this->pipelines.clear();
}

void RenderManager::addPipeline(const char* vertexPath, const char* fragmentPath,
    const char* geometryPath, const char* computePath, const char* tessControlPath,
    const char* tessEvalPath) {
    RenderPipeline newPipeline = RenderPipeline(
        vertexPath, fragmentPath, geometryPath, computePath, tessControlPath, tessEvalPath);

    this->pipelines.push_back(newPipeline);
}

void RenderManager::updateMatrices(int* width, int* height) {
    // Initialize matrices
    this->modelMatrix = glm::mat4(1.0f);
    // this->viewMatrix = glm::lookAt(glm::vec3(2.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
    // glm::vec3(0.0f, 1.0f, 0.0f));
    this->viewMatrix = camera->getViewMatrix();
    this->projectionMatrix =
        glm::perspective(glm::radians(45.0f), (float)*width / (float)*height, 0.01f, 100.0f);
}

void RenderManager::loadScene() {

    // TODO: Remove this later and get these values from load-model
    //  Define the cube's vertices positions and colors separately
    GLfloat cubePositions[] = {-0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f};

    GLfloat cubeColors[] = {1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    // Define the cube's vertex indices
    GLuint cubeIndices[] = {
        0, 1, 2, 2, 3, 0, // Front face
        1, 5, 6, 6, 2, 1, // Right face
        4, 0, 3, 3, 7, 4, // Left face
        5, 4, 7, 7, 6, 5, // Back face
        3, 2, 6, 6, 7, 3, // Top face
        4, 5, 1, 1, 0, 4 // Bottom face
    };

    GLfloat cubeNormals[] = {
        // Front face
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,
        0.0f,  0.0f, -1.0f,

        // Right face
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,

        // Left face
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,
       -1.0f,  0.0f,  0.0f,

       // Back face
       0.0f,  0.0f,  1.0f,
       0.0f,  0.0f,  1.0f,
       0.0f,  0.0f,  1.0f,
       0.0f,  0.0f,  1.0f,

       // Top face
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,
       0.0f,  1.0f,  0.0f,

       // Bottom face
       0.0f, -1.0f,  0.0f,
       0.0f, -1.0f,  0.0f,
       0.0f, -1.0f,  0.0f,
       0.0f, -1.0f,  0.0f,
    };

    //ADD LIGHT SOURCES

    this->addLightSource(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    ///////////////////////////////////////////////////////

    // TODO: change path
    const char* colorVertexPath = "assets/shaders/colours.vert";
    const char* colorFragPath = "assets/shaders/colours.frag";

    // TODO: Should probably be called in the Constructor
    // Should be made in the order of Enum Pipeline
    addPipeline(colorVertexPath, colorFragPath); // ColorPipeline - 0

    // create vertex buffer object(VBO)
    VertexBuffer posVBO(sizeof(cubePositions), cubePositions, PositionsBuffer);
    VertexBuffer colVBO(sizeof(cubeColors), cubeColors, ColorsBuffer);
    VertexBuffer normalVBO(sizeof(cubeNormals), cubeNormals, NormalsBuffer);

    // create an element buffer object for the indices
    IndexBuffer EBO(sizeof(cubeIndices), cubeIndices);

    // TODO: (Not sure how to manage the below)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderManager::renderScene(Camera* camera, GLFWwindow* window) {
    // get window dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    // update matrices
    updateMatrices(&width, &height);

    // adapt to resize
    glViewport(0, 0, width, height);

    // draw background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    runPipeline(ColourPipeline);
}

void RenderManager::renderSceneRefactor(Camera* camera, int width, int height) {
    // update matrices
    updateMatrices(&width, &height);

    // draw background
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    runPipeline(ColourPipeline);
}

void RenderManager::runPipeline(Pipeline pipeline) {
    switch (pipeline) {
    case ColourPipeline:
        runColourPipeline();
        break;
    case TexturePipeline:
        runTexturePipeline();
        break;
    case ShadowPipeline:
        runShadowPipeline();
        break;
    case BillboardPipeline:
        runBillboardPipeline();
        break;
    case WaterPipeline:
        runWaterPipeline();
        break;
    case Render2DPipeline:
        run2DPipeline();
        break;
    }
}

void RenderManager::setupColourPipelineUniforms()
{
    getPipeline(ColourPipeline)->setUniformLocations();
}

void RenderManager::runColourPipeline() {
    glUseProgram(getPipeline(ColourPipeline)->getProgram());

    //sending uniform data
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getModelID(), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getProjectionID(), 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniform3f(getPipeline(ColourPipeline)->getLightPosID(),
        lights[0].getPosition().x,
        lights[0].getPosition().y,
        lights[0].getPosition().z);
    glUniform3f(getPipeline(ColourPipeline)->getLightColID(),
        lights[0].getColour().x,
        lights[0].getColour().y,
        lights[0].getColour().z);

    //// Render the cube
    glBindVertexArray(getPipeline(ColourPipeline)->getVAO());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // TODO: Does this go here? - ALEX: VAO is unbound before next operation
}

void RenderManager::addLightSource(glm::vec3& position, glm::vec3& colour)
{
    lights.emplace_back(position, colour);
}

RenderPipeline* RenderManager::getPipeline(std::size_t index) {
    return &this->pipelines[index];
}

LightSource* RenderManager::getLightSource(std::size_t index) {
    return &this->lights[index];
}


void RenderManager::runTexturePipeline() {
}

void RenderManager::runShadowPipeline() {
}

void RenderManager::runBillboardPipeline() {
}
void RenderManager::runWaterPipeline() {
}
void RenderManager::run2DPipeline() {
}
