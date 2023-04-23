
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
    glEnable(GL_LINE_SMOOTH);
    glClearColor(0.2f, 0.2f, .2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL);
    glLineWidth(1.0f);
    glfwSwapBuffers(aWindow);
    glfwPollEvents();

    this->pipelines.clear();
    this->deltaTime = 0.0f;
    this->xPos = 0.0f;
    this->yPos = 0.0f;
    this->xPosLast = 0.0f;
    this->yPosLast = 0.0f;
    this->firstRClick = true;
    this->xOffset = 0.0f;
    this->yOffset = 0.0f;

    // Initialise the camera
    this->camera = Camera(glm::vec3(.0f, 2.f, 8.f), glm::vec3(.0f, -2.f, -8.f));

    this->modelMatrix = glm::mat4(1.0f);
    this->viewMatrix = glm::lookAt(
        glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    this->projectionMatrix = glm::mat4(1.0f);
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
    this->viewMatrix = camera.getViewMatrix();
    this->projectionMatrix = glm::perspective(
        glm::radians(camera.fov / 2.0f), (float)*width / (float)*height, 0.01f, 100.0f);
    previewProjectionMatrix = glm::perspective(
        glm::radians(previewCamera.fov / 2.0f), (float)*width / (float)*height, 0.01f, 100.0f);
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

    ///////////////////////////////////////////////////////
    const char* colorVertexPath = "assets/shaders/colours.vert";
    const char* colorFragPath = "assets/shaders/colours.frag";
    const char* gridVertPath = "assets/shaders/grid.vert";
    const char* gridFragPath = "assets/shaders/grid.frag";
    const char* frustumVisVertPath = "assets/shaders/frustum.vert";

    // TODO: Should probably be called in the Constructor
    // Should be made in the order of Enum Pipeline
    addPipeline(colorVertexPath, colorFragPath); // ColorPipeline - 0

    // create vertex buffer object(VBO)
    VertexBuffer posVBO(sizeof(cubePositions), cubePositions, PositionsBuffer);
    VertexBuffer colVBO(sizeof(cubeColors), cubeColors, ColorsBuffer);

    // create an element buffer object for the indices
    IndexBuffer EBO(sizeof(cubeIndices), cubeIndices);

    // TODO: Decide whether we are doing fixed number of pipelines or an
    // arbitrary number. This is incredibly inconvenient and hard to maintain.
    // skip unimplemented stuff
    pipelines.resize(GridPipeline);
    addPipeline(gridVertPath, gridFragPath);
    addPipeline(frustumVisVertPath, gridFragPath);

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
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    runPipeline(ColorPipeline);
}

void RenderManager::renderSceneRefactor(Camera* camera, int width, int height) {
    // update matrices
    updateMatrices(&width, &height);

    // draw background
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    runPipeline(ColorPipeline);
}

void RenderManager::renderEntities(const Scene& scene, Camera* camera, int width, int height) {
    updateMatrices(&width, &height);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        modelMatrix = scene.entities[i].runtimeTransform;
        runPipeline(ColorPipeline);
    }

    // draw preview camera frustum
    if (scene.selectedEntity) {
        modelMatrix = scene.selectedEntity->runtimeTransform;
        for (unsigned int i = 0; i < scene.selectedEntity->components.vecCameraComponent.size();
             i++) {
            CameraComponent& cam = scene.selectedEntity->components.vecCameraComponent[i];
            previewCamera.setDirect(cam.eye, cam.center, cam.up, cam.fov);
            runFrustumVisPipeline();
        }
    }
}
void RenderManager::renderGrid(int width, int height) {
    updateMatrices(&width, &height);

    runGridPipeline();
}

void RenderManager::runPipeline(Pipeline pipeline) {
    switch (pipeline) {
    case ColorPipeline:
        runColorPipeline();
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
    default:;
    }
}

void RenderManager::runColorPipeline() {
    glUseProgram(getPipeline(ColorPipeline)->getProgram());

    ////handle for uniforms
    GLuint ModelID = glGetUniformLocation(getPipeline(ColorPipeline)->getProgram(), "model");
    GLuint ViewID = glGetUniformLocation(getPipeline(ColorPipeline)->getProgram(), "view");
    GLuint ProjectionID =
        glGetUniformLocation(getPipeline(ColorPipeline)->getProgram(), "projection");

    glUniformMatrix4fv(ModelID, 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(ViewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &projectionMatrix[0][0]);

    //// Render the cube
    glBindVertexArray(getPipeline(ColorPipeline)->getVAO());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
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
void RenderManager::runGridPipeline() {
    // TODO more maintainable way to get pipeline
    RenderPipeline pipeline = pipelines[GridPipeline];
    glUseProgram(pipeline.getProgram());

    const int GRIDSIZE = 60;

    // get uniform ids
    GLuint gridSizeID = glGetUniformLocation(pipeline.getProgram(), "size");
    GLuint offsetID = glGetUniformLocation(pipeline.getProgram(), "offset");
    GLuint ViewID = glGetUniformLocation(pipeline.getProgram(), "view");
    GLuint ProjectionID = glGetUniformLocation(pipeline.getProgram(), "projection");

    // upload uniforms
    glm::vec3 camCenter = camera.getCenter();
    glm::vec3 offset;
    glm::modf(camCenter, offset);
    glUniform1i(gridSizeID, GRIDSIZE);
    glUniform2f(offsetID, offset.x, offset.z);
    glUniformMatrix4fv(ViewID, 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &projectionMatrix[0][0]);

    // Render the grid
    glBindVertexArray(pipeline.getVAO());
    glDrawArrays(GL_LINES, 0, 4 * GRIDSIZE + 4);
}
void RenderManager::runFrustumVisPipeline() {
    RenderPipeline pipeline = pipelines[FrustumVisPipeline];
    glUseProgram(pipeline.getProgram());

    // get uniform ids
    GLuint mvpID = glGetUniformLocation(pipeline.getProgram(), "MVP");
    GLuint previewInverseID = glGetUniformLocation(pipeline.getProgram(), "previewInverse");

    // upload uniforms
    glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;
    glm::mat4 previewInverse =
        glm::inverse(previewProjectionMatrix * previewCamera.getViewMatrix());
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(previewInverseID, 1, GL_FALSE, &previewInverse[0][0]);

    // Render the grid
    glBindVertexArray(pipeline.getVAO());
    glDrawArrays(GL_LINES, 0, 24);
}
