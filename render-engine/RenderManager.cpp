
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

    //Load entity models
    Model model;
    model.loadModel("assets/tree.obj");
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
    //for (unsigned int i = 0; i < model.meshes.size(); i++)
    //{
    //    VertexBuffer *vertexBuffer;
    //    vertexBuffer = VertexBuffer::VertexBuffer(model.meshes[i].vertices.size(), model.meshes[i].vertices);
    //}
    VertexBuffer vBuffer(model.meshes[0].vertices.size(), &model.meshes[0].vertices, TexturedObjectBuffer);
        
    //VertexBuffer posVBO(sizeof(cubePositions), cubePositions, PositionsBuffer);
    

    // Set up the VAO
    //GLuint VAO;
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    //// Set up the VBO for positions, colors, and normals
    //GLuint VBO;
    //glGenBuffers(1, &VBO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    //// Set up the vertex attributes
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(0));
    //glEnableVertexAttribArray(1);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));

    // create an element buffer object for the indices
    IndexBuffer EBO(model.meshes[0].indices.size() * sizeof(int), &model.meshes[0].indices);

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
