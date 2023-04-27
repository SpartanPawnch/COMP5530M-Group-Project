
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
    glEnable(GL_MULTISAMPLE);
    glClearColor(0.2f, 0.2f, .2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPolygonMode(GL_FRONT, GL_FILL);
    glLineWidth(1.0f);
    glfwSwapBuffers(aWindow);
    glfwPollEvents();

    this->pipelines.clear();
    this->pipelines.resize(Pipeline_MAX);
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
    this->camera = Camera(glm::vec3(.0f, 2.f, 8.f), glm::vec3(.0f, -2.f, -8.f));

    this->modelMatrix = glm::mat4(1.0f);
    this->viewMatrix = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    this->projectionMatrix = glm::mat4(1.0f);
}

void RenderManager::shutDown() {
    this->pipelines.clear();
}

void RenderManager::addPipeline(Pipeline pipe, const char* vertexPath, const char* fragmentPath,
    const char* geometryPath, const char* computePath, const char* tessControlPath,
    const char* tessEvalPath) {
    RenderPipeline newPipeline = RenderPipeline(vertexPath, fragmentPath, geometryPath, computePath,
        tessControlPath, tessEvalPath);

    this->pipelines[pipe] = newPipeline;
}

void RenderManager::updateMatrices(int* width, int* height) {
    // Initialize matrices
    this->modelMatrix = glm::mat4(1.0f);
    // this->viewMatrix = glm::lookAt(glm::vec3(2.0f, 4.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
    // glm::vec3(0.0f, 1.0f, 0.0f));
    this->viewMatrix = camera.getViewMatrix();
    this->projectionMatrix = glm::perspective(glm::radians(camera.fov / 2.0f),
        (float)*width / (float)*height, 0.01f, 100.0f);
}

void RenderManager::addMeshToPipeline(std::vector<Pipeline> pipeline, VertexBuffer vBuffer,
    IndexBuffer iBuffer, GLuint VAO) {
    for (unsigned int i = 0; i < pipeline.size(); i++) {
        if (PipelineMeshBufferMap.find(pipeline[i]) == PipelineMeshBufferMap.end()) {
            PipelineMeshBufferMap[pipeline[i]] = std::vector<Buffer>{Buffer(vBuffer, iBuffer)};
        }
        else {
            PipelineMeshBufferMap[pipeline[i]].push_back(Buffer(vBuffer, iBuffer));
        }

        pipelines[pipeline[i]].addVAO(VAO);
    }
}

void RenderManager::loadScene() {
    // Load entity models
    Model model;
    model.loadModel("assets/tree.obj");

    // clang-format off
    GLfloat cubeVertices[] = {
        // Front face
        // Position                 // Normal               // Color
        -1.0f,-1.0f,5.0f,           0.0f,0.0f,1.0f,         1.0f,0.0f,0.0f,
        1.0f,-1.0f,5.0f,            0.0f,0.0f,1.0f,         0.0f,1.0f,0.0f,
        1.0f,1.0f,5.0f,             0.0f,0.0f,1.0f,         0.0f,0.0f,1.0f,
        -1.0f,1.0f,5.0f,            0.0f,0.0f,1.0f,         1.0f,1.0f,0.0f,
        // Back face
        // Position                 // Normal               // Color
        -1.0f,-1.0f,-5.0f,          0.0f,0.0f,-1.0f,        1.0f,0.0f,1.0f,
        1.0f,-1.0f,-5.0f,           0.0f,0.0f,-1.0f,        0.0f,1.0f,1.0f,
        1.0f,1.0f,-5.0f,            0.0f,0.0f,-1.0f,        1.0f,1.0f,1.0f,
        -1.0f,1.0f,-5.0f,           0.0f,0.0f,-1.0f,        0.0f,0.0f,0.0f,
        // Left face
        // Position                 // Normal               // Color
        -1.0f,-1.0f,-5.0f,          -1.0f,0.0f,0.0f,        1.0f,0.0f,1.0f,
        -1.0f,-1.0f,5.0f,           -1.0f,0.0f,0.0f,        1.0f,0.0f,0.0f,
        -1.0f,1.0f,5.0f,            -1.0f,0.0f,0.0f,        1.0f,1.0f,0.0f,
        -1.0f,1.0f,-5.0f,           -1.0f,0.0f,0.0f,        1.0f,1.0f,1.0f,
        // Right face
        // Position                 // Normal               // Color
        1.0f,-1.0f,-5.0f,           1.0f,0.0f,0.0f,         0.0f,1.0f,1.0f,
        1.0f,-1.0f,5.0f,            1.0f,0.0f,0.0f,         0.0f,1.0f,0.0f,
        1.0f,1.0f,5.0f,             1.0f,0.0f,0.0f,         0.0f,0.0f,1.0f,
        1.0f,1.0f,-5.0f,            1.0f,0.0f,0.0f,         1.0f,1.0f,1.0f,
        // Top face
        // Position                 // Normal               // Color
        -1.0f,1.0f,5.0f,            0.0f,1.0f,0.0f,         1.0f,1.0f,0.0f,
        1.0f,1.0f,5.0f,             0.0f,1.0f,0.0f,         0.0f,0.0f,1.0f,
        1.0f,1.0f,-5.0f,            0.0f,1.0f,0.0f,         1.0f,1.0f,1.0f,
        -1.0f,1.0f,-5.0f,           0.0f,1.0f,0.0f,         0.0f,0.0f,0.0f,
        // Bottom face
        // Position                 // Normal               // Color
        -1.0f,-1.0f,5.0f,           0.0f,-1.0f,0.0f,        1.0f,0.0f,0.0f,
        1.0f,-1.0f,5.0f,            0.0f,-1.0f,0.0f,        0.0f,1.0f,0.0f,
        1.0f,-1.0f,-5.0f,           0.0f,-1.0f,0.0f,        0.0f,1.0f,1.0f,
        -1.0f,-1.0f,-5.0f,          0.0f,-1.0f,0.0f,        1.0f,0.0f,1.0f,
    };
    // clang-format on
    GLuint cubeIndices[] = {
        0, 1, 2, 2, 3,
        0, // Front face
        4, 5, 6, 6, 7,
        4, // Back face
        8, 9, 10, 10, 11,
        8, // Left face
        12, 13, 14, 14, 15,
        12, // Right face
        16, 17, 18, 18, 19,
        16, // Top face
        20, 21, 22, 22, 23,
        20 // Bottom face
    };

    std::vector<float> allVertices;
    std::vector<unsigned int> allIndices;
    allVertices.clear();
    allIndices.clear();
    for (std::size_t i = 0; i < model.meshes[0].vertices.size(); i++) {
        allVertices.push_back(model.meshes[0].vertices[i].position.x);
        allVertices.push_back(model.meshes[0].vertices[i].position.y);
        allVertices.push_back(model.meshes[0].vertices[i].position.z);
        allVertices.push_back(model.meshes[0].vertices[i].normal.x);
        allVertices.push_back(model.meshes[0].vertices[i].normal.y);
        allVertices.push_back(model.meshes[0].vertices[i].normal.z);
        allVertices.push_back(model.meshes[0].vertices[i].texCoords.x);
        allVertices.push_back(model.meshes[0].vertices[i].texCoords.y);
    }
    for (std::size_t i = 0; i < model.meshes[0].indices.size(); i++) {
        allIndices.push_back(model.meshes[0].indices[i]);
    }

    // ADD LIGHT SOURCES

    this->addLightSource(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

    ///////////////////////////////////////////////////////
    const char* colorVertexPath = "assets/shaders/colours.vert";
    const char* colorFragPath = "assets/shaders/colours.frag";
    const char* gridVertPath = "assets/shaders/grid.vert";
    const char* gridFragPath = "assets/shaders/grid.frag";
    const char* frustumVisVertPath = "assets/shaders/frustum.vert";
    const char* texVertexPath = "assets/shaders/tex.vert";
    const char* texFragPath = "assets/shaders/tex.frag";
    const char* emptyVertPath = "assets/shaders/empty.vert";
    const char* emptyFragPath = "assets/shaders/empty.frag";

    // TODO: Should probably be called in the Constructor
    // Should be made in the order of Enum Pipeline
    addPipeline(ColourPipeline, colorVertexPath, colorFragPath); // ColorPipeline - 0
    // Vertex Array Object
    GLuint VAO1;
    glGenVertexArrays(1, &VAO1);
    glBindVertexArray(VAO1);

    VertexBuffer cubeBuffer(sizeof(cubeVertices), cubeVertices, ColoredObjectBuffer);
    IndexBuffer EBO(sizeof(cubeIndices), cubeIndices);
    addMeshToPipeline(std::vector<Pipeline>{ColourPipeline}, cubeBuffer, EBO, VAO1);

    addPipeline(TexturePipeline, texVertexPath, texFragPath);
    // std::cout << "we have " << model.meshes.size() << " meshes in model\n";
    // for (std::size_t i = 0; i < model.meshes[0].vertices.size(); i++)
    //{
    //     std::cout << model.meshes[0].vertices[i].position.x << " "
    //         << model.meshes[0].vertices[i].position.y << " "
    //         << model.meshes[0].vertices[i].position.z << std::endl;
    // }
    // std::cout << "We have " << model.meshes[0].indices.size() << " indices and " <<
    // model.meshes[0].indices.size() / 3 << "triangles";
    GLuint VAO2;
    glGenVertexArrays(1, &VAO2);
    glBindVertexArray(VAO2);

    VertexBuffer vBuffer(model.meshes[0].vertices.size() * sizeof(Vertex),
        &model.meshes[0].vertices[0], TexturedObjectBuffer);
    IndexBuffer ebo(model.meshes[0].indices.size() * sizeof(unsigned int),
        &model.meshes[0].indices[0]);
    addMeshToPipeline(std::vector<Pipeline>{TexturePipeline}, vBuffer, ebo, VAO2);

    // add vis pipelines
    addPipeline(GridPipeline, gridVertPath, gridFragPath);
    addPipeline(FrustumVisPipeline, frustumVisVertPath, gridFragPath);
    addPipeline(EmptyVisPipeline, emptyVertPath, emptyFragPath);

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
    // TODO: Also check if the pipeline has models
    if (this->pipelines[ColourPipeline].initialised == true) {
        runPipeline(ColourPipeline);
    }
    if (this->pipelines[TexturePipeline].initialised == true) {
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

void RenderManager::renderEntities(const Scene& scene, Camera* camera, int width, int height) {
    // updateMatrices(&width, &height);
    viewMatrix = camera->getViewMatrix();
    float aspect = float(width) / height;
    aspect = (glm::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0) &&
                 !(aspect != aspect))
        ? aspect
        : 1.f;

    projectionMatrix = glm::perspective(glm::radians(camera->fov / 2.f), aspect, .01f, 100.0f);

    // draw background
    // glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        modelMatrix = scene.entities[i].state.runtimeTransform;

        for (unsigned int j = 0; j < scene.entities[i].components.vecModelComponent.size(); j++) {
            auto desc = scene.entities[i].components.vecModelComponent[j].modelDescriptor;
            if (!desc) {
                continue;
            }
            for (unsigned int k = 0; k < desc->getMeshCount(); k++) {

                GLuint tex = desc->getTexture(k);
                if (tex) {
                    // draw textured
                    // bind model matrix
                    glUseProgram(getPipeline(TexturePipeline)->getProgram());

                    glUniformMatrix4fv(getPipeline(TexturePipeline)->getModelID(), 1, GL_FALSE,
                        &modelMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(TexturePipeline)->getViewID(), 1, GL_FALSE,
                        &viewMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(TexturePipeline)->getProjectionID(), 1, GL_FALSE,
                        &projectionMatrix[0][0]);
                    glUniform3f(getPipeline(TexturePipeline)->getLightPosID(),
                        lights[0].getPosition().x, lights[0].getPosition().y,
                        lights[0].getPosition().z);
                    glUniform3f(getPipeline(TexturePipeline)->getLightColID(),
                        lights[0].getColour().x, lights[0].getColour().y, lights[0].getColour().z);

                    glBindTexture(GL_TEXTURE_2D, tex);
                }
                else {
                    // TODO use vertex colours
                    // draw untextured
                    // bind model matrix
                    glUseProgram(getPipeline(ColourPipeline)->getProgram());

                    glUniformMatrix4fv(getPipeline(ColourPipeline)->getModelID(), 1, GL_FALSE,
                        &modelMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(ColourPipeline)->getViewID(), 1, GL_FALSE,
                        &viewMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(ColourPipeline)->getProjectionID(), 1, GL_FALSE,
                        &projectionMatrix[0][0]);
                    glUniform3f(getPipeline(ColourPipeline)->getLightPosID(),
                        lights[0].getPosition().x, lights[0].getPosition().y,
                        lights[0].getPosition().z);
                    glUniform3f(getPipeline(ColourPipeline)->getLightColID(),
                        lights[0].getColour().x, lights[0].getColour().y, lights[0].getColour().z);
                }
                glBindVertexArray(desc->getVAO(k));
                glDrawElements(GL_TRIANGLES, desc->getIndexCount(k), GL_UNSIGNED_INT, 0);
            }
        }

        if (scene.entities[i].components.vecModelComponent.empty()) {
            runEmptyVisPipeline();
        }

        glBindVertexArray(0);
    }
}
void RenderManager::renderGrid(Camera* camera, int width, int height) {
    // updateMatrices(&width, &height);
    viewMatrix = camera->getViewMatrix();
    float aspect = float(width) / height;
    aspect = (glm::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0) &&
                 !(aspect != aspect))
        ? aspect
        : 1.f;

    projectionMatrix = glm::perspective(glm::radians(camera->fov / 2.f), aspect, .01f, 100.0f);

    runGridPipeline();
}

void RenderManager::renderCamPreview(const Scene& scene, int width, int height) {
    // draw preview camera frustum
    if (scene.selectedEntity) {
        modelMatrix = scene.selectedEntity->state.runtimeTransform;
        for (unsigned int i = 0; i < scene.selectedEntity->components.vecCameraComponent.size();
             i++) {
            CameraComponent& cam = scene.selectedEntity->components.vecCameraComponent[i];
            float aspect = float(width) / height;
            aspect =
                (glm::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0) &&
                    !(aspect != aspect))
                ? aspect
                : 1.f;
            previewMatrix = glm::perspective(glm::radians(cam.fov / 2.0f), aspect, .01f, 100.f) *
                glm::lookAt(cam.eye, cam.center, cam.up);
            runFrustumVisPipeline();
        }
    }
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
    default:;
    }
}

void RenderManager::setupColourPipelineUniforms() {
    getPipeline(ColourPipeline)->setUniformLocations();
}

void RenderManager::runColourPipeline() {
    glUseProgram(getPipeline(ColourPipeline)->getProgram());

    // sending uniform data
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getModelID(), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);
    glUniform3f(getPipeline(ColourPipeline)->getLightPosID(), lights[0].getPosition().x,
        lights[0].getPosition().y, lights[0].getPosition().z);
    glUniform3f(getPipeline(ColourPipeline)->getLightColID(), lights[0].getColour().x,
        lights[0].getColour().y, lights[0].getColour().z);

    //// Render the cube
    for (unsigned int i = 0; i < getPipeline(ColourPipeline)->getNoOfMeshes(); i++) {
        glBindVertexArray(getPipeline(ColourPipeline)->getVAO(i));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // TODO:: Change 36 to dynamic
    }

    glBindVertexArray(0);
}

void RenderManager::setupTexturePipelineUniforms() {
    getPipeline(TexturePipeline)->setUniformLocations();
}

void RenderManager::runTexturePipeline() {
    glUseProgram(getPipeline(TexturePipeline)->getProgram());

    // sending uniform data
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getModelID(), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);
    glUniform3f(getPipeline(TexturePipeline)->getLightPosID(), lights[0].getPosition().x,
        lights[0].getPosition().y, lights[0].getPosition().z);
    glUniform3f(getPipeline(TexturePipeline)->getLightColID(), lights[0].getColour().x,
        lights[0].getColour().y, lights[0].getColour().z);

    //// Render the cube
    for (unsigned int i = 0; i < getPipeline(TexturePipeline)->getNoOfMeshes(); i++) {
        glBindVertexArray(getPipeline(TexturePipeline)->getVAO(i));
        glDrawElements(GL_TRIANGLES, 2136, GL_UNSIGNED_INT, 0); // TODO:: Change 36 to dynamic
    }

    glBindVertexArray(0);
}

void RenderManager::addLightSource(glm::vec3& position, glm::vec3& colour) {
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
    glBindVertexArray(pipelines[ColourPipeline].getVAO(0));
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
    glm::mat4 previewInverse = glm::inverse(previewMatrix);
    glUniformMatrix4fv(mvpID, 1, GL_FALSE, &mvp[0][0]);
    glUniformMatrix4fv(previewInverseID, 1, GL_FALSE, &previewInverse[0][0]);

    // Render the grid
    glBindVertexArray(pipelines[ColourPipeline].getVAO(0));
    glDrawArrays(GL_LINES, 0, 16);
}

void RenderManager::runEmptyVisPipeline() {
    RenderPipeline pipeline = pipelines[EmptyVisPipeline];
    glUseProgram(pipeline.getProgram());

    // get uniform ids
    GLuint mvID = glGetUniformLocation(pipeline.getProgram(), "modelView");
    GLuint projID = glGetUniformLocation(pipeline.getProgram(), "proj");

    // upload uniforms
    glm::mat4 mv = viewMatrix * modelMatrix;
    glm::mat4 proj = projectionMatrix;
    glUniformMatrix4fv(mvID, 1, GL_FALSE, &mv[0][0]);
    glUniformMatrix4fv(projID, 1, GL_FALSE, &proj[0][0]);

    // render empty
    glBindVertexArray(pipelines[ColourPipeline].getVAO(0));
    glDrawArrays(GL_LINES, 0, 6);
}

// adapted from https://learnopengl.com/Model-Loading/Mesh
void RenderManager::uploadMesh(std::vector<Vertex>* v, std::vector<unsigned int>* i,
    unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) {
    glGenVertexArrays(1, VAO);
    glGenBuffers(1, VBO);
    glGenBuffers(1, EBO);

    glBindVertexArray(*VAO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);

    glBufferData(GL_ARRAY_BUFFER, (*v).size() * sizeof(Vertex), &(*v)[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, (*i).size() * sizeof(unsigned int), &(*i)[0],
        GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void RenderManager::deleteMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) {
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);
}
