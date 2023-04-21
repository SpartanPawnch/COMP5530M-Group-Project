
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
    this->pipelines.resize(6);
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

void RenderManager::addPipeline( Pipeline pipe, const char* vertexPath, const char* fragmentPath,
    const char* geometryPath, const char* computePath, const char* tessControlPath,
    const char* tessEvalPath) {
    RenderPipeline newPipeline = RenderPipeline(
        vertexPath, fragmentPath, geometryPath, computePath, tessControlPath, tessEvalPath);

    this->pipelines[pipe] = newPipeline;
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

void RenderManager::addMeshToPipeline(std::vector<Pipeline> pipeline, VertexBuffer vBuffer, IndexBuffer iBuffer, GLuint VAO)
{
    for (unsigned int i = 0; i < pipeline.size(); i++)
    {
        if (PipelineMeshBufferMap.find(pipeline[i]) == PipelineMeshBufferMap.end())
        {
            PipelineMeshBufferMap[pipeline[i]] = std::vector <Buffer>{ Buffer(vBuffer, iBuffer) };
        }
        else
        {
            PipelineMeshBufferMap[pipeline[i]].push_back(Buffer(vBuffer, iBuffer));
        }

        pipelines[pipeline[i]].addVAO(VAO);
    }
   
}

void RenderManager::loadScene() {

    // Probably the job of Asset Manager
    ///////////////////////////////////
    //Load entity models
    Model model;
    model.loadModel("assets/tree.obj");

    //////////////////////////////////

    GLfloat cubeVertices[] = {
        // Front face
        // Position                 // Normal               // Color  
        -1.0f, -1.0f,  5.0f,         0.0f, 0.0f, 1.0f,      1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  5.0f,         0.0f, 0.0f, 1.0f,      0.0f, 1.0f, 0.0f,
         1.0f,  1.0f,  5.0f,         0.0f, 0.0f, 1.0f,      0.0f, 0.0f, 1.0f,
        -1.0f,  1.0f,  5.0f,         0.0f, 0.0f, 1.0f,      1.0f, 1.0f, 0.0f,
        // Back face 
        // Position                 // Normal               // Color  
        -1.0f, -1.0f, -5.0f,         0.0f, 0.0f, -1.0f,     1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -5.0f,         0.0f, 0.0f, -1.0f,     0.0f, 1.0f, 1.0f,
         1.0f,  1.0f, -5.0f,         0.0f, 0.0f, -1.0f,     1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -5.0f,         0.0f, 0.0f, -1.0f,     0.0f, 0.0f, 0.0f,
        // Left face
        // Position                 // Normal               // Color  
        -1.0f, -1.0f, -5.0f,         -1.0f, 0.0f, 0.0f,     1.0f, 0.0f, 1.0f,
        -1.0f, -1.0f,  5.0f,         -1.0f, 0.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f,  5.0f,         -1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,
        -1.0f,  1.0f, -5.0f,         -1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f,
        // Right face
        // Position                 // Normal               // Color  
        1.0f, -1.0f, -5.0f,          1.0f, 0.0f, 0.0f,      0.0f, 1.0f, 1.0f,
        1.0f, -1.0f,  5.0f,          1.0f, 0.0f, 0.0f,      0.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  5.0f,          1.0f, 0.0f, 0.0f,      0.0f, 0.0f, 1.0f,
        1.0f,  1.0f, -5.0f,          1.0f, 0.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        // Top face
        // Position                 // Normal               // Color  
        -1.0f,  1.0f,  5.0f,         0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 0.0f,
        1.0f,  1.0f,  5.0f,          0.0f, 1.0f, 0.0f,      0.0f, 0.0f, 1.0f,
        1.0f,  1.0f, -5.0f,          0.0f, 1.0f, 0.0f,      1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, -5.0f,         0.0f, 1.0f, 0.0f,      0.0f, 0.0f, 0.0f,
        // Bottom face
        // Position                 // Normal               // Color         
        -1.0f, -1.0f,  5.0f,         0.0f, -1.0f, 0.0f,     1.0f, 0.0f, 0.0f,
        1.0f, -1.0f,  5.0f,          0.0f, -1.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, -5.0f,          0.0f, -1.0f, 0.0f,     0.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, -5.0f,         0.0f, -1.0f, 0.0f,     1.0f, 0.0f, 1.0f,
    };
    GLuint cubeIndices[] = {
        0, 1, 2, 2, 3, 0,      // Front face
        4, 5, 6, 6, 7, 4,      // Back face
        8, 9, 10, 10, 11, 8,   // Left face
        12, 13, 14, 14, 15, 12, // Right face
        16, 17, 18, 18, 19, 16, // Top face
        20, 21, 22, 22, 23, 20  // Bottom face
    };


 

    //ADD LIGHT SOURCES
    this->addLightSource(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    ///////////////////////////////////////////////////////

    // TODO: change path
    const char* colorVertexPath = "assets/shaders/colours.vert";
    const char* colorFragPath = "assets/shaders/colours.frag";
    const char* texVertexPath = "assets/shaders/tex.vert";
    const char* texFragPath = "assets/shaders/tex.frag";

    // TODO: Should probably be called in the Constructor
    // Should be made in the order of Enum Pipeline
    addPipeline(ColourPipeline, colorVertexPath, colorFragPath); // ColorPipeline - 0
    //Vertex Array Object
    GLuint VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    VertexBuffer cubeBuffer(sizeof(cubeVertices), cubeVertices, ColoredObjectBuffer);
    IndexBuffer EBO(sizeof(cubeIndices), cubeIndices);
    addMeshToPipeline(std::vector<Pipeline>{ColourPipeline}, cubeBuffer, EBO, VAO1);

    addPipeline(TexturePipeline,texVertexPath, texFragPath);
    GLuint VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    VertexBuffer vBuffer(model.meshes[0].vertices.size()* sizeof(Vertex), &model.meshes[0].vertices[0], TexturedObjectBuffer);
    IndexBuffer ebo(model.meshes[0].indices.size() * sizeof(unsigned int), &model.meshes[0].indices[0]);
    addMeshToPipeline(std::vector<Pipeline>{TexturePipeline}, vBuffer, ebo, VAO2);

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
    glShadeModel(GL_FLAT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    runPipeline(ColourPipeline);
}

void RenderManager::renderSceneRefactor(Camera* camera, int width, int height) {
    // update matrices
    updateMatrices(&width, &height);

    // draw background
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    //TODO: Also check if the pipeline has models
    if (this->pipelines[ColourPipeline].initialised == true)
    {
        runPipeline(ColourPipeline);
    }
    if (this->pipelines[TexturePipeline].initialised == true)
    {
        runPipeline(TexturePipeline);
    }
 /*   if (this->pipelines[ShadowPipeline].initialised == true)
    {
        runPipeline(ShadowPipeline);
    }
    if (this->pipelines[BillboardPipeline].initialised == true)
    {
        runPipeline(BillboardPipeline);
    }
    if (this->pipelines[WaterPipeline].initialised == true)
    {
        runPipeline(WaterPipeline);
    }
    if (this->pipelines[Render2DPipeline].initialised == true)
    {
        runPipeline(Render2DPipeline);
    }*/

    
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
    for (unsigned int i = 0; i < getPipeline(ColourPipeline)->getNoOfMeshes(); i++)
    {
        glBindVertexArray(getPipeline(ColourPipeline)->getVAO(i));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); //TODO:: Change 36 to dynamic
    }
    
    glBindVertexArray(0); 
}

void RenderManager::setupTexturePipelineUniforms()
{
    getPipeline(TexturePipeline)->setUniformLocations();
}

void RenderManager::runTexturePipeline() {
    glUseProgram(getPipeline(TexturePipeline)->getProgram());

    //sending uniform data
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getModelID(), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getProjectionID(), 1, GL_FALSE, &projectionMatrix[0][0]);
    glUniform3f(getPipeline(TexturePipeline)->getLightPosID(),
        lights[0].getPosition().x,
        lights[0].getPosition().y,
        lights[0].getPosition().z);
    glUniform3f(getPipeline(TexturePipeline)->getLightColID(),
        lights[0].getColour().x,
        lights[0].getColour().y,
        lights[0].getColour().z);

    //// Render the cube
    for (unsigned int i = 0; i < getPipeline(TexturePipeline)->getNoOfMeshes(); i++)
    {
        glBindVertexArray(getPipeline(TexturePipeline)->getVAO(i));
        glDrawElements(GL_TRIANGLES, 2136, GL_UNSIGNED_INT, 0); //TODO:: Change 36 to dynamic
    }
    
    glBindVertexArray(0); 
}

void RenderManager::addLightSource(glm::vec3& position, glm::vec3& colour)
{
    lights.emplace_back(position, colour);
}

RenderPipeline* RenderManager::getPipeline(Pipeline pipe) {
    return &this->pipelines[pipe];
}

LightSource* RenderManager::getLightSource(std::size_t index) {
    return &this->lights[index];
}

void RenderManager::runShadowPipeline() {
}

void RenderManager::runBillboardPipeline() {
}
void RenderManager::runWaterPipeline() {
}
void RenderManager::run2DPipeline() {
}
