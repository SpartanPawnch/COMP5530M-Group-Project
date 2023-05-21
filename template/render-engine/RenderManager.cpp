#include "RenderManager.h"
#include <glm/gtx/transform.hpp>

RenderManager* RenderManager::getInstance() {
    if (instance == nullptr) {
        instance = new RenderManager();
        // std::cout << "Render Instance created now." << std::endl;
    }
    // else {
    //     std::cout << "Render instance has been created previously. Returning previous instance."
    //               << std::endl;
    // }
    return instance;
}

// empty VAO for pipelines without attributes
static GLuint dummyVAO;

void RenderManager::startUp(GLFWwindow* aWindow) {
    physicsEngine = PhysicsEngine::getInstance();
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
    this->directionalLights.clear();
    this->deltaTime = 0.0f;
    this->xPos = 0.0f;
    this->yPos = 0.0f;
    this->xPosLast = 0.0f;
    this->yPosLast = 0.0f;
    this->firstRClick = true;
    this->xOffset = 0.0f;
    this->yOffset = 0.0f;

    // function to set gamma correction - value sent to all pipelines
    setGammaCorrection(2.2f);

    // Enable antialiasing
    this->antialiasingEnabled = true;
    if (this->antialiasingEnabled == true) {
        glEnable(GL_MULTISAMPLE);
    }

    // Initialise the camera
    this->camera = Camera(glm::vec3(.0f, 2.f, 8.f), glm::vec3(.0f, -2.f, -8.f));

    this->modelMatrix = glm::mat4(1.0f);
    this->viewMatrix = glm::lookAt(glm::vec3(0.0f, 3.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f));
    this->projectionMatrix = glm::mat4(1.0f);

    // generate visualization primitives
    glGenVertexArrays(1, &dummyVAO);
}

void RenderManager::shutDown() {
    this->pipelines.clear();
    glDeleteVertexArrays(1, &dummyVAO);
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
            PipelineMeshBufferMap[pipeline[i]] = std::vector<Buffer>{ Buffer(vBuffer, iBuffer) };
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

    // ADD LIGHT SOURCES

    // this->addLightSource(glm::vec3(10.0f, 10.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f),
    //     glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f));
    // this->addLightSource(glm::vec3(-10.0f, -10.0f, 1.0f), glm::vec3(0.2f, 0.2f, 0.2f),
    //     glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f));
    //
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
    const char* AnimatedVertexPath = "assets/shaders/animated.vert";
    const char* cubemapVertPath = "assets/shaders/cubemap.vert";
    const char* cubemapFragPath = "assets/shaders/cubemap.frag";
    const char* iconVertPath = "assets/shaders/icon.vert";
    const char* iconFragPath = "assets/shaders/icon.frag";
    const char* entIDVertexPath = "assets/shaders/entID.vert";
    const char* entIDFragPath = "assets/shaders/entID.frag";
    const char* iconIDVertexPath = "assets/shaders/iconID.vert";
    const char* iconIDFragPath = "assets/shaders/iconID.frag";
    const char* cubeColliderVertexPath = "assets/shaders/cubewire.vert";
    const char* sphereColliderVertexPath = "assets/shaders/spherewire.vert";
    const char* capsuleColliderVertexPath = "assets/shaders/capsulewire.vert";
    const char* colliderFragPath = "assets/shaders/collider.frag";

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
    addPipeline(EntIDPipeline, entIDVertexPath, entIDFragPath);
    addPipeline(AnimatedPipeline, AnimatedVertexPath, texFragPath);
    addPipeline(CubemapPipeline, cubemapVertPath, cubemapFragPath);
    addPipeline(IconPipeline, iconVertPath, iconFragPath);
    addPipeline(IconIDPipeline, iconIDVertexPath, iconIDFragPath);
    addPipeline(AnimationIDPipeline, AnimatedVertexPath, entIDFragPath);
    addPipeline(CubeColliderPipeline, cubeColliderVertexPath, colliderFragPath);
    addPipeline(SphereColliderPipeline, sphereColliderVertexPath, colliderFragPath);
    addPipeline(CapsuleColliderPipeline, capsuleColliderVertexPath, colliderFragPath);
    // TODO: (Not sure how to manage the below)
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // load the dummy texture for runtime only.
    // Why do we have like 10 different init functions for RenderManager?

#ifndef ONO_ENGINE_ONLY
    pixelImageDescriptor = loadTexture("assets/pixel.png", "assets/pixel.png");
#endif
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
    glShadeModel(GL_SMOOTH);

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

void RenderManager::movePhysicsEntities(Scene& scene, Camera* camera, int width, int height) {
    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        if (scene.entities[i].components.vecRigidBodyComponent.size() == 0) {
            continue;
        }
        const Transform& transform =
            scene.entities[i].components.vecRigidBodyComponent[0].rigidBody->getTransform();

        const Vector3& position = transform.getPosition();
        const Quaternion& orientation = transform.getOrientation();

        glm::vec3 translation(position.x, position.y, position.z);
        glm::quat rotation(orientation.w, orientation.x, orientation.y, orientation.z);
        // glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0f), translation) *
        // glm::toMat4(rotation);

        // scene.entities[i].state.runtimeTransform = modelMatrix;

        scene.entities[i].state.position = translation;
        scene.entities[i].state.rotation = rotation;
    }
}

static GLuint skyboxID;

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

#ifdef ONO_ENGINE_ONLY
        if (scene.entities[i].components.vecModelComponent.empty() &&
            scene.entities[i].components.vecSkeletalModelComponent.empty() &&
            scene.entities[i].components.vecSkyBoxComponent.empty()) {
            if (!scene.entities[i].components.vecCameraComponent.empty() && cameraIconDescriptor) {
                glUseProgram(getPipeline(IconPipeline)->getProgram());

                glBindTexture(GL_TEXTURE_2D, cameraIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (!scene.entities[i].components.vecLightComponent.empty() &&
                ligthIconDescriptor) {
                glUseProgram(getPipeline(IconPipeline)->getProgram());

                // bind texture
                glBindTexture(GL_TEXTURE_2D, ligthIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (!scene.entities[i].components.vecDirectionalLightComponent.empty() &&
                ligthIconDescriptor) {
                glUseProgram(getPipeline(IconPipeline)->getProgram());

                // bind texture
                glBindTexture(GL_TEXTURE_2D, ligthIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (!scene.entities[i].components.vecAudioSourceComponent.empty() &&
                soundIconDescriptor) {
                glUseProgram(getPipeline(IconPipeline)->getProgram());

                glBindTexture(GL_TEXTURE_2D, soundIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else {
                runEmptyVisPipeline();
            }
            continue;
        }
#endif

        if (!scene.entities[i].visible)
            continue;

        for (unsigned int j = 0; j < scene.entities[i].components.vecModelComponent.size(); j++) {
            auto desc = scene.entities[i].components.vecModelComponent[j].modelDescriptor;
            if (!desc) {
                continue;
            }
            for (unsigned int k = 0; k < desc->getMeshCount(); k++) {
                if (scene.entities[i].components.vecModelComponent[j].materials[k]) {
                    // draw textured
                    glUseProgram(getPipeline(TexturePipeline)->getProgram());

                    // set MVP
                    glUniformMatrix4fv(getPipeline(TexturePipeline)->getModelID(), 1, GL_FALSE,
                        &modelMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(TexturePipeline)->getViewID(), 1, GL_FALSE,
                        &viewMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(TexturePipeline)->getProjectionID(), 1, GL_FALSE,
                        &projectionMatrix[0][0]);

                    // set universal lighting uniforms
                    glUniform3f(getPipeline(TexturePipeline)->getViewPosID(),
                        this->camera.getPosition().x, this->camera.getPosition().y,
                        this->camera.getPosition().z);
                    glUniform1f(getPipeline(TexturePipeline)->getGammaID(), gammaValue);
                    glUniform1i(getPipeline(TexturePipeline)->getNumLightsID(),
                        static_cast<int>(this->lights.size()));

                    // set per-light uniforms
                    for (std::size_t i = 0; i < lights.size(); i++) {
                        glUniform3f(getPipeline(TexturePipeline)->getLightPosID(i),
                            lights[i].getPosition().x, lights[i].getPosition().y,
                            lights[i].getPosition().z);
                        glUniform3f(getPipeline(TexturePipeline)->getLightAmbientID(i),
                            lights[i].getAmbient().x, lights[i].getAmbient().y,
                            lights[i].getAmbient().z);
                        glUniform3f(getPipeline(TexturePipeline)->getLightDiffuseID(i),
                            lights[i].getDiffuse().x, lights[i].getDiffuse().y,
                            lights[i].getDiffuse().z);
                        glUniform3f(getPipeline(TexturePipeline)->getLightSpecularID(i),
                            lights[i].getSpecular().x, lights[i].getSpecular().y,
                            lights[i].getSpecular().z);
                    }

                    glUniform1i(getPipeline(TexturePipeline)->getNumDirectionalLightsID(),
                        static_cast<int>(this->directionalLights.size()));
                    // set per-light uniforms
                    for (std::size_t i = 0; i < directionalLights.size(); i++) {
                        glUniform3f(getPipeline(TexturePipeline)->getDirectionalLightPosID(i),
                            directionalLights[i].getPosition().x,
                            directionalLights[i].getPosition().y,
                            directionalLights[i].getPosition().z);
                        glUniform3f(getPipeline(TexturePipeline)->getDirectionalLightAmbientID(i),
                            directionalLights[i].getAmbient().x,
                            directionalLights[i].getAmbient().y,
                            directionalLights[i].getAmbient().z);
                        glUniform3f(getPipeline(TexturePipeline)->getDirectionalLightDiffuseID(i),
                            directionalLights[i].getDiffuse().x,
                            directionalLights[i].getDiffuse().y,
                            directionalLights[i].getDiffuse().z);
                        glUniform3f(getPipeline(TexturePipeline)->getDirectionalLightSpecularID(i),
                            directionalLights[i].getSpecular().x,
                            directionalLights[i].getSpecular().y,
                            directionalLights[i].getSpecular().z);
                    }

                    std::shared_ptr<ActiveMaterial> meshMat =
                        scene.entities[i].components.vecModelComponent[j].materials[k];

                    glUniform3f(getPipeline(TexturePipeline)->getBaseColorID(),
                        meshMat->baseColor.x, meshMat->baseColor.y, meshMat->baseColor.z);
                    glUniform3f(getPipeline(TexturePipeline)->getEmissiveColorID(),
                        meshMat->emissiveColor.x, meshMat->emissiveColor.y,
                        meshMat->emissiveColor.z);

                    glUniform1f(getPipeline(TexturePipeline)->getRoughnessID(), meshMat->roughness);
                    glUniform1f(getPipeline(TexturePipeline)->getMetalnessID(), meshMat->metalness);
                    glUniform1f(getPipeline(TexturePipeline)->getOcclusionID(), meshMat->occlusion);

                    glUniform1i(getPipeline(TexturePipeline)->getbaseColorSamplerLocationID(), 0);
                    glUniform1i(getPipeline(TexturePipeline)->getroughnessSamplerLocationID(), 1);
                    glUniform1i(getPipeline(TexturePipeline)->getmetalnessSamplerLocationID(), 2);
                    glUniform1i(getPipeline(TexturePipeline)->getnormalSamplerLocationID(), 3);
                    glUniform1i(getPipeline(TexturePipeline)->getalphaSamplerLocationID(), 4);
                    glUniform1i(getPipeline(TexturePipeline)->getemissiveSamplerLocationID(), 5);
                    glUniform1i(getPipeline(TexturePipeline)->getocclusionSamplerLocationID(), 6);

                    // todo: send 1x1 white image on the elses of all
                    glActiveTexture(GL_TEXTURE0);
                    if (meshMat->baseColorMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->baseColorMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE1);
                    if (meshMat->roughnessMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->roughnessMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE2);
                    if (meshMat->metalnessMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->metalnessMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE3);
                    if (meshMat->normalMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->normalMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE4);
                    if (meshMat->alphaMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->alphaMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE5);
                    if (meshMat->emissiveMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->emissiveMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE6);
                    if (meshMat->occlusionMap)
                        glBindTexture(GL_TEXTURE_2D, meshMat->occlusionMap->texId);
                    else
                        glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                    glActiveTexture(GL_TEXTURE7);
                    glBindTexture(GL_TEXTURE_CUBE_MAP,
                        skyboxID);

                    glActiveTexture(GL_TEXTURE0);
                }
                else {
                    // TODO use vertex colours
                    // draw untextured
                    // bind model matrix
                    glUseProgram(getPipeline(ColourPipeline)->getProgram());

                    // set MVP
                    glUniformMatrix4fv(getPipeline(ColourPipeline)->getModelID(), 1, GL_FALSE,
                        &modelMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(ColourPipeline)->getViewID(), 1, GL_FALSE,
                        &viewMatrix[0][0]);
                    glUniformMatrix4fv(getPipeline(ColourPipeline)->getProjectionID(), 1, GL_FALSE,
                        &projectionMatrix[0][0]);

                    // set universal lighting uniforms
                    glUniform3f(getPipeline(ColourPipeline)->getViewPosID(),
                        this->camera.getPosition().x, this->camera.getPosition().y,
                        this->camera.getPosition().z);
                    glUniform1f(getPipeline(ColourPipeline)->getGammaID(), gammaValue);
                    glUniform1i(getPipeline(ColourPipeline)->getNumLightsID(),
                        static_cast<int>(this->lights.size()));

                    // set per-light uniforms
                    for (std::size_t l = 0; l < lights.size(); l++) {
                        glUniform3f(getPipeline(ColourPipeline)->getLightPosID(l),
                            lights[l].getPosition().x, lights[l].getPosition().y,
                            lights[l].getPosition().z);
                        glUniform3f(getPipeline(ColourPipeline)->getLightAmbientID(l),
                            lights[l].getAmbient().x, lights[l].getAmbient().y,
                            lights[l].getAmbient().z);
                        glUniform3f(getPipeline(ColourPipeline)->getLightDiffuseID(l),
                            lights[l].getDiffuse().x, lights[l].getDiffuse().y,
                            lights[l].getDiffuse().z);
                        glUniform3f(getPipeline(ColourPipeline)->getLightSpecularID(k),
                            lights[l].getSpecular().x, lights[l].getSpecular().y,
                            lights[l].getSpecular().z);
                    }
                }
                glBindVertexArray(desc->getVAO(k));
                glDrawElements(GL_TRIANGLES, desc->getIndexCount(k), GL_UNSIGNED_INT, 0);
            }
        }

        glUseProgram(getPipeline(AnimatedPipeline)->getProgram());

        // bind model matrix
        glUniformMatrix4fv(getPipeline(AnimatedPipeline)->getModelID(), 1, GL_FALSE,
            &modelMatrix[0][0]);
        glUniformMatrix4fv(getPipeline(AnimatedPipeline)->getViewID(), 1, GL_FALSE,
            &viewMatrix[0][0]);
        glUniformMatrix4fv(getPipeline(AnimatedPipeline)->getProjectionID(), 1, GL_FALSE,
            &projectionMatrix[0][0]);

        // set universal light uniforms
        glUniform3f(getPipeline(AnimatedPipeline)->getViewPosID(), this->camera.getPosition().x,
            this->camera.getPosition().y, this->camera.getPosition().z);
        glUniform1f(getPipeline(AnimatedPipeline)->getGammaID(), gammaValue);
        glUniform1i(getPipeline(AnimatedPipeline)->getNumLightsID(),
            static_cast<int>(this->lights.size()));

        // set per-light uniforms
        for (std::size_t k = 0; k < lights.size(); k++) {
            glUniform3f(getPipeline(AnimatedPipeline)->getLightPosID(k), lights[k].getPosition().x,
                lights[k].getPosition().y, lights[k].getPosition().z);
            glUniform3f(getPipeline(AnimatedPipeline)->getLightAmbientID(k),
                lights[k].getAmbient().x, lights[k].getAmbient().y, lights[k].getAmbient().z);
            glUniform3f(getPipeline(AnimatedPipeline)->getLightDiffuseID(k),
                lights[k].getDiffuse().x, lights[k].getDiffuse().y, lights[k].getDiffuse().z);
            glUniform3f(getPipeline(AnimatedPipeline)->getLightSpecularID(k),
                lights[k].getSpecular().x, lights[k].getSpecular().y, lights[k].getSpecular().z);
        }

        glUniform1i(getPipeline(AnimatedPipeline)->getNumDirectionalLightsID(),
            static_cast<int>(this->directionalLights.size()));
        // set per-light uniforms
        for (std::size_t i = 0; i < directionalLights.size(); i++) {
            glUniform3f(getPipeline(AnimatedPipeline)->getDirectionalLightPosID(i),
                directionalLights[i].getPosition().x, directionalLights[i].getPosition().y,
                directionalLights[i].getPosition().z);
            glUniform3f(getPipeline(AnimatedPipeline)->getDirectionalLightAmbientID(i),
                directionalLights[i].getAmbient().x, directionalLights[i].getAmbient().y,
                directionalLights[i].getAmbient().z);
            glUniform3f(getPipeline(AnimatedPipeline)->getDirectionalLightDiffuseID(i),
                directionalLights[i].getDiffuse().x, directionalLights[i].getDiffuse().y,
                directionalLights[i].getDiffuse().z);
            glUniform3f(getPipeline(AnimatedPipeline)->getDirectionalLightSpecularID(i),
                directionalLights[i].getSpecular().x, directionalLights[i].getSpecular().y,
                directionalLights[i].getSpecular().z);
        }

        for (unsigned int j = 0; j < scene.entities[i].components.vecSkeletalModelComponent.size();
            j++) {
            auto desc = scene.entities[i].components.vecSkeletalModelComponent[j].modelDescriptor;
            if (!desc) {
                continue;
            }
            for (unsigned int k = 0; k < desc->getMeshCount(); k++) {
                glUniformMatrix4fv(getPipeline(AnimatedPipeline)->getBonesMatrix(), 100, GL_FALSE,
                    &scene.entities[i]
                    .components.vecSkeletalModelComponent[j]
                    .transformMatrices[0][0][0]);

                // glBindTexture(GL_TEXTURE_2D, desc->getTexture(k));
                std::shared_ptr<ActiveMaterial> meshMat =
                    scene.entities[i].components.vecSkeletalModelComponent[j].materials[k];

                glUniform3f(getPipeline(TexturePipeline)->getBaseColorID(), meshMat->baseColor.x,
                    meshMat->baseColor.y, meshMat->baseColor.z);
                glUniform3f(getPipeline(TexturePipeline)->getEmissiveColorID(),
                    meshMat->emissiveColor.x, meshMat->emissiveColor.y, meshMat->emissiveColor.z);

                glUniform1f(getPipeline(TexturePipeline)->getRoughnessID(), meshMat->roughness);
                glUniform1f(getPipeline(TexturePipeline)->getMetalnessID(), meshMat->metalness);
                glUniform1f(getPipeline(TexturePipeline)->getOcclusionID(), meshMat->occlusion);

                glUniform1i(getPipeline(TexturePipeline)->getbaseColorSamplerLocationID(), 0);
                glUniform1i(getPipeline(TexturePipeline)->getroughnessSamplerLocationID(), 1);
                glUniform1i(getPipeline(TexturePipeline)->getmetalnessSamplerLocationID(), 2);
                glUniform1i(getPipeline(TexturePipeline)->getnormalSamplerLocationID(), 3);
                glUniform1i(getPipeline(TexturePipeline)->getalphaSamplerLocationID(), 4);
                glUniform1i(getPipeline(TexturePipeline)->getemissiveSamplerLocationID(), 5);
                glUniform1i(getPipeline(TexturePipeline)->getocclusionSamplerLocationID(), 6);

                // todo: send 1x1 white image on the elses of all
                glActiveTexture(GL_TEXTURE0);
                if (meshMat->baseColorMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->baseColorMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE1);
                if (meshMat->roughnessMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->roughnessMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE2);
                if (meshMat->metalnessMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->metalnessMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE3);
                if (meshMat->normalMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->normalMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE4);
                if (meshMat->alphaMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->alphaMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE5);
                if (meshMat->emissiveMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->emissiveMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE6);
                if (meshMat->occlusionMap)
                    glBindTexture(GL_TEXTURE_2D, meshMat->occlusionMap->texId);
                else
                    glBindTexture(GL_TEXTURE_2D, pixelImageDescriptor->texId);

                glActiveTexture(GL_TEXTURE7);
                glBindTexture(GL_TEXTURE_CUBE_MAP,
                    skyboxID);

                glActiveTexture(GL_TEXTURE0);

                glBindVertexArray(desc->getVAO(k));
                glDrawElements(GL_TRIANGLES, desc->getIndexCount(k), GL_UNSIGNED_INT, 0);
            }
        }

        glBindVertexArray(0);
    }
}

void RenderManager::renderEntitiesID(const Scene& scene, Camera* camera, int width, int height) {
    updateMatrices(&width, &height);

    // draw background
    // glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // RENDERING
    // Go through all the Pipelines
    // TODO: Check if it is necessary to use the given pipeline and the call the following fn
    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        modelMatrix = scene.entities[i].state.runtimeTransform;

        int entityIndex = i + 1;

        int colorX = (entityIndex & 0x000000FF) >> 0;
        int colorY = (entityIndex & 0x0000FF00) >> 8;
        int colorZ = (entityIndex & 0x00FF0000) >> 16;

        glm::vec3 reconstructed_color = glm::vec3(colorX / 255.0, colorY / 255.0, colorZ / 255.0);

        if (scene.entities[i].components.vecModelComponent.empty() &&
            scene.entities[i].components.vecSkeletalModelComponent.empty()) {
            if (!scene.entities[i].components.vecCameraComponent.empty() && cameraIconDescriptor) {
                glUseProgram(getPipeline(IconIDPipeline)->getProgram());

                glUniform3f(getPipeline(IconIDPipeline)->getEntID(), reconstructed_color.x,
                    reconstructed_color.y, reconstructed_color.z);

                glBindTexture(GL_TEXTURE_2D, cameraIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (!scene.entities[i].components.vecLightComponent.empty() &&
                ligthIconDescriptor) {
                glUseProgram(getPipeline(IconIDPipeline)->getProgram());

                glUniform3f(getPipeline(IconIDPipeline)->getEntID(), reconstructed_color.x,
                    reconstructed_color.y, reconstructed_color.z);

                // bind texture
                glBindTexture(GL_TEXTURE_2D, ligthIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (!scene.entities[i].components.vecDirectionalLightComponent.empty() &&
                ligthIconDescriptor) {
                glUseProgram(getPipeline(IconIDPipeline)->getProgram());

                glUniform3f(getPipeline(IconIDPipeline)->getEntID(), reconstructed_color.x,
                    reconstructed_color.y, reconstructed_color.z);

                // bind texture
                glBindTexture(GL_TEXTURE_2D, ligthIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else if (!scene.entities[i].components.vecAudioSourceComponent.empty() &&
                soundIconDescriptor) {
                glUseProgram(getPipeline(IconIDPipeline)->getProgram());

                glUniform3f(getPipeline(IconIDPipeline)->getEntID(), reconstructed_color.x,
                    reconstructed_color.y, reconstructed_color.z);

                glBindTexture(GL_TEXTURE_2D, soundIconDescriptor->texId);

                // bind matrices
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getModelID(), 1, GL_FALSE,
                    &modelMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getViewID(), 1, GL_FALSE,
                    &viewMatrix[0][0]);
                glUniformMatrix4fv(getPipeline(IconIDPipeline)->getProjectionID(), 1, GL_FALSE,
                    &projectionMatrix[0][0]);

                // render square with icon texture
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
            }
            else {
                runEmptyVisPipeline();
            }
            continue;
        }

        glUseProgram(getPipeline(EntIDPipeline)->getProgram());

        // bind model matrix
        glUniformMatrix4fv(getPipeline(EntIDPipeline)->getModelID(), 1, GL_FALSE,
            &modelMatrix[0][0]);
        glUniformMatrix4fv(getPipeline(EntIDPipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
        glUniformMatrix4fv(getPipeline(EntIDPipeline)->getProjectionID(), 1, GL_FALSE,
            &projectionMatrix[0][0]);
        glUniform3f(getPipeline(EntIDPipeline)->getEntID(), reconstructed_color.x,
            reconstructed_color.y, reconstructed_color.z);

        for (unsigned int j = 0; j < scene.entities[i].components.vecModelComponent.size(); j++) {
            auto desc = scene.entities[i].components.vecModelComponent[j].modelDescriptor;
            if (!desc) {
                continue;
            }
            for (unsigned int k = 0; k < desc->getMeshCount(); k++) {

                glBindVertexArray(desc->getVAO(k));
                glDrawElements(GL_TRIANGLES, desc->getIndexCount(k), GL_UNSIGNED_INT, 0);
            }
        }

        glUseProgram(getPipeline(AnimationIDPipeline)->getProgram());

        // bind model matrix
        glUniformMatrix4fv(getPipeline(AnimationIDPipeline)->getModelID(), 1, GL_FALSE,
            &modelMatrix[0][0]);
        glUniformMatrix4fv(getPipeline(AnimationIDPipeline)->getViewID(), 1, GL_FALSE,
            &viewMatrix[0][0]);
        glUniformMatrix4fv(getPipeline(AnimationIDPipeline)->getProjectionID(), 1, GL_FALSE,
            &projectionMatrix[0][0]);
        glUniform3f(getPipeline(AnimationIDPipeline)->getEntID(), reconstructed_color.x,
            reconstructed_color.y, reconstructed_color.z);

        for (unsigned int j = 0; j < scene.entities[i].components.vecSkeletalModelComponent.size();
            j++) {
            auto desc = scene.entities[i].components.vecSkeletalModelComponent[j].modelDescriptor;
            if (!desc) {
                continue;
            }
            for (unsigned int k = 0; k < desc->getMeshCount(); k++) {
                glUniformMatrix4fv(getPipeline(AnimationIDPipeline)->getBonesMatrix(), 100,
                    GL_FALSE,
                    &scene.entities[i]
                    .components.vecSkeletalModelComponent[j]
                    .transformMatrices[0][0][0]);

                glBindVertexArray(desc->getVAO(k));
                glDrawElements(GL_TRIANGLES, desc->getIndexCount(k), GL_UNSIGNED_INT, 0);
            }
        }
        glBindVertexArray(0);
    }
}

void RenderManager::renderSkybox(const Scene& scene, Camera* camera, int width, int height) {
    // updateMatrices(&width, &height);
    viewMatrix = camera->getViewMatrix();
    float aspect = float(width) / height;
    aspect = (glm::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0) &&
        !(aspect != aspect))
        ? aspect
        : 1.f;

    projectionMatrix = glm::perspective(glm::radians(camera->fov / 2.f), aspect, .01f, 100.0f);

    glUseProgram(getPipeline(CubemapPipeline)->getProgram());
    // steal first VAO from textured pipeline
    glBindVertexArray(dummyVAO);

    for (unsigned int i = 0; i < scene.entities.size(); i++) {
        for (unsigned int j = 0; j < scene.entities[i].components.vecSkyBoxComponent.size(); j++) {
            if (scene.entities[i].components.vecSkyBoxComponent[j].skybox.id == 0)
                continue;

            glUniformMatrix4fv(getPipeline(CubemapPipeline)->getViewID(), 1, GL_FALSE,
                glm::value_ptr(glm::mat4(glm::mat3(viewMatrix))));
            glUniformMatrix4fv(getPipeline(CubemapPipeline)->getProjectionID(), 1, GL_FALSE,
                &projectionMatrix[0][0]);

            glDepthMask(GL_FALSE);
            glBindTexture(GL_TEXTURE_CUBE_MAP,
                scene.entities[i].components.vecSkyBoxComponent[j].skybox.id);
            skyboxID = scene.entities[i].components.vecSkyBoxComponent[j].skybox.id;
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            glDepthMask(GL_TRUE);
        }
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

void RenderManager::renderColliders(const Scene& scene, int width, int height) {
    viewMatrix = camera.getViewMatrix();
    float aspect = float(width) / height;
    aspect = (glm::abs(aspect - std::numeric_limits<float>::epsilon()) > static_cast<float>(0) &&
        !(aspect != aspect))
        ? aspect
        : 1.f;

    projectionMatrix = glm::perspective(glm::radians(camera.fov / 2.f), aspect, .01f, 100.0f);

    // cube colliders
    glUseProgram(getPipeline(CubeColliderPipeline)->getProgram());
    glUniformMatrix4fv(getPipeline(CubeColliderPipeline)->getViewID(), 1, false, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(CubeColliderPipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);

    for (size_t i = 0; i < scene.entities.size(); i++) {
        for (size_t j = 0; j < scene.entities[i].components.vecRigidBodyComponent.size(); j++) {
            const RigidBodyComponent& rigidBody =
                scene.entities[i].components.vecRigidBodyComponent[j];
            glm::mat4 baseTransform = scene.entities[i].state.runtimeTransform *
                glm::scale(glm::vec3(1.f) / scene.entities[i].state.scale);
            for (size_t k = 0; k < rigidBody.cubeColliders.size(); k++) {
                modelMatrix = baseTransform * glm::translate(rigidBody.cubeColliders[k].position) *
                    glm::mat4_cast(rigidBody.cubeColliders[k].rotation) *
                    glm::scale(rigidBody.cubeColliders[k].extents);
                glUniformMatrix4fv(getPipeline(CubeColliderPipeline)->getModelID(), 1, false,
                    &modelMatrix[0][0]);
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_LINES, 0, 24);
            }
        }
    }

    // sphere colliders
    glUseProgram(getPipeline(SphereColliderPipeline)->getProgram());
    glUniformMatrix4fv(getPipeline(SphereColliderPipeline)->getViewID(), 1, false,
        &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(SphereColliderPipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);

    for (size_t i = 0; i < scene.entities.size(); i++) {
        for (size_t j = 0; j < scene.entities[i].components.vecRigidBodyComponent.size(); j++) {
            const RigidBodyComponent& rigidBody =
                scene.entities[i].components.vecRigidBodyComponent[j];
            glm::mat4 baseTransform = scene.entities[i].state.runtimeTransform *
                glm::scale(glm::vec3(1.f) / scene.entities[i].state.scale);
            for (size_t k = 0; k < rigidBody.sphereColliders.size(); k++) {
                modelMatrix = baseTransform *
                    glm::translate(rigidBody.sphereColliders[k].position) *
                    glm::mat4_cast(rigidBody.sphereColliders[k].rotation) *
                    glm::scale(glm::vec3(rigidBody.sphereColliders[k].colliderShape->getRadius()));
                glUniformMatrix4fv(getPipeline(SphereColliderPipeline)->getModelID(), 1, false,
                    &modelMatrix[0][0]);
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_LINES, 0, 600);
            }
        }
    }

    // capsule colliders
    glUseProgram(getPipeline(CapsuleColliderPipeline)->getProgram());
    glUniformMatrix4fv(getPipeline(CapsuleColliderPipeline)->getViewID(), 1, false,
        &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(CapsuleColliderPipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);

    for (size_t i = 0; i < scene.entities.size(); i++) {
        for (size_t j = 0; j < scene.entities[i].components.vecRigidBodyComponent.size(); j++) {
            const RigidBodyComponent& rigidBody =
                scene.entities[i].components.vecRigidBodyComponent[j];
            glm::mat4 baseTransform = scene.entities[i].state.runtimeTransform *
                glm::scale(glm::vec3(1.f) / scene.entities[i].state.scale);
            for (size_t k = 0; k < rigidBody.capsuleColliders.size(); k++) {
                modelMatrix = baseTransform *
                    glm::translate(rigidBody.capsuleColliders[k].position) *
                    glm::mat4_cast(rigidBody.capsuleColliders[k].rotation);
                glUniformMatrix4fv(getPipeline(CapsuleColliderPipeline)->getModelID(), 1, false,
                    &modelMatrix[0][0]);
                glUniform1f(glGetUniformLocation(getPipeline(CapsuleColliderPipeline)->getProgram(),
                    "radius"),
                    rigidBody.capsuleColliders[k].colliderShape->getRadius());
                glUniform1f(glGetUniformLocation(getPipeline(CapsuleColliderPipeline)->getProgram(),
                    "height"),
                    rigidBody.capsuleColliders[k].colliderShape->getHeight());
                glBindVertexArray(dummyVAO);
                glDrawArrays(GL_LINES, 0, 1208);
            }
        }
    }
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
    getPipeline(ColourPipeline)->setColourUniformLocations();
}

void RenderManager::runColourPipeline() {
    glUseProgram(getPipeline(ColourPipeline)->getProgram());

    // sending uniform data
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getModelID(), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(ColourPipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);
    glUniform3f(getPipeline(ColourPipeline)->getViewPosID(), this->camera.getPosition().x,
        this->camera.getPosition().y, this->camera.getPosition().z);
    glUniform1f(getPipeline(ColourPipeline)->getGammaID(), gammaValue);
    glUniform1i(getPipeline(ColourPipeline)->getNumLightsID(),
        static_cast<int>(this->lights.size()));

    for (std::size_t i = 0; i < lights.size(); i++) {
        glUniform3f(getPipeline(ColourPipeline)->getLightPosID(i), lights[i].getPosition().x,
            lights[i].getPosition().y, lights[i].getPosition().z);
        glUniform3f(getPipeline(ColourPipeline)->getLightAmbientID(i), lights[i].getAmbient().x,
            lights[i].getAmbient().y, lights[i].getAmbient().z);
        glUniform3f(getPipeline(ColourPipeline)->getLightDiffuseID(i), lights[i].getDiffuse().x,
            lights[i].getDiffuse().y, lights[i].getDiffuse().z);
        glUniform3f(getPipeline(ColourPipeline)->getLightSpecularID(i), lights[i].getSpecular().x,
            lights[i].getSpecular().y, lights[i].getSpecular().z);
    }

    //// Render the cube
    for (unsigned int i = 0; i < getPipeline(ColourPipeline)->getNoOfMeshes(); i++) {
        glBindVertexArray(getPipeline(ColourPipeline)->getVAO(i));
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0); // TODO:: Change 36 to dynamic
    }

    glBindVertexArray(0);
}

void RenderManager::setupTexturePipelineUniforms() {
    getPipeline(TexturePipeline)->setUniformLocations();
    getPipeline(TexturePipeline)->setTextureUniformLocations();
}

void RenderManager::setupAnimatedPipelineUniforms() {
    getPipeline(AnimatedPipeline)->setTextureUniformLocations();
    getPipeline(AnimatedPipeline)->setUniformLocations();
}
void RenderManager::setupEntIDPipelineUniforms() {
    getPipeline(EntIDPipeline)->setIDUniformLocations();
}

void RenderManager::setupIconIDPipelineUniforms() {
    getPipeline(IconIDPipeline)->setIDUniformLocations();
}

void RenderManager::setupAnimationIDPipelineUniforms() {
    getPipeline(AnimationIDPipeline)->setIDUniformLocations();
}

void RenderManager::setupCubemapPipelineUniforms() {
    getPipeline(CubemapPipeline)->setUniformLocations();
}

void RenderManager::setupIconPipelineUniforms() {
    getPipeline(IconPipeline)->setUniformLocations();
    // can't be bothered to write another function,
    // this should have probably gone in pipeline constructor anyways
    getPipeline(CubeColliderPipeline)->setUniformLocations();
    getPipeline(SphereColliderPipeline)->setUniformLocations();
    getPipeline(CapsuleColliderPipeline)->setUniformLocations();
}

void RenderManager::loadIcons() {
    // load necessary icons
    ligthIconDescriptor = loadTexture("assets/lightico.png", "assets/lightico.png");
    cameraIconDescriptor = loadTexture("assets/cameraico.png", "assets/cameraico.png");
    soundIconDescriptor = loadTexture("assets/soundico.png", "assets/soundico.png");
    pixelImageDescriptor = loadTexture("assets/pixel.png", "assets/pixel.png");
}

void RenderManager::runTexturePipeline() {
    glUseProgram(getPipeline(TexturePipeline)->getProgram());

    // sending uniform data
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getModelID(), 1, GL_FALSE, &modelMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getViewID(), 1, GL_FALSE, &viewMatrix[0][0]);
    glUniformMatrix4fv(getPipeline(TexturePipeline)->getProjectionID(), 1, GL_FALSE,
        &projectionMatrix[0][0]);
    glUniform1f(getPipeline(TexturePipeline)->getGammaID(), gammaValue);
    glUniform1i(getPipeline(TexturePipeline)->getNumLightsID(),
        static_cast<int>(this->lights.size()));
    // std::cout << this->lights.size() << std::endl;

    for (std::size_t i = 0; i < lights.size(); i++) {
        glUniform3f(getPipeline(TexturePipeline)->getLightPosID(i), lights[i].getPosition().x,
            lights[i].getPosition().y, lights[i].getPosition().z);
        glUniform3f(getPipeline(TexturePipeline)->getLightAmbientID(i), lights[i].getAmbient().x,
            lights[i].getAmbient().y, lights[i].getAmbient().z);
        glUniform3f(getPipeline(TexturePipeline)->getLightDiffuseID(i), lights[i].getDiffuse().x,
            lights[i].getDiffuse().y, lights[i].getDiffuse().z);
        glUniform3f(getPipeline(TexturePipeline)->getLightSpecularID(i), lights[i].getSpecular().x,
            lights[i].getSpecular().y, lights[i].getSpecular().z);
    }

    //// Render the model
    glBindVertexArray(getPipeline(TexturePipeline)->getVAO(0));
    glDrawElements(GL_TRIANGLES, 2136, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

std::shared_ptr<LightDescriptor> RenderManager::addLightSource(glm::vec3& position,
    glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular) {
    lights.emplace_back(position, ambient, diffuse, specular);
    std::shared_ptr<LightDescriptor> ptr = std::make_shared<LightDescriptor>();
    lightsMetadata.emplace_back(std::weak_ptr<LightDescriptor>(ptr));
    ptr->idx = lights.size() - 1;
    return ptr;
}

void RenderManager::removeLightSource(size_t idx) {
    if (lights.size() < idx)
        return;
    // swap with back
    std::swap(lights[idx], lights.back());
    std::swap(lightsMetadata[idx], lightsMetadata.back());

    // fix index
    lightsMetadata[idx].lock()->idx = idx;

    // delete
    lightsMetadata.pop_back();
    lights.pop_back();
}

std::shared_ptr<LightDescriptor> RenderManager::addDirectionalLightSource(glm::vec3& direction,
    glm::vec3& ambient, glm::vec3& diffuse, glm::vec3& specular) {
    directionalLights.emplace_back(direction, ambient, diffuse, specular);
    std::shared_ptr<LightDescriptor> ptr = std::make_shared<LightDescriptor>();
    directionalLightsMetadata.emplace_back(std::weak_ptr<LightDescriptor>(ptr));
    ptr->idx = directionalLights.size() - 1;
    return ptr;
}

void RenderManager::removeDirectionalLightSource(size_t idx) {
    if (directionalLights.size() < idx)
        return;
    // swap with back
    std::swap(directionalLights[idx], directionalLights.back());
    std::swap(directionalLightsMetadata[idx], directionalLightsMetadata.back());

    // fix index
    directionalLightsMetadata[idx].lock()->idx = idx;

    // delete
    directionalLightsMetadata.pop_back();
    directionalLights.pop_back();
}

RenderPipeline* RenderManager::getPipeline(Pipeline pipe) {
    return &this->pipelines[pipe];
}

LightSource* RenderManager::getLightSource(std::size_t index) {
    return &this->lights[index];
}

LightSource* RenderManager::getDirectionalLightSource(std::size_t index) {
    return &this->directionalLights[index];
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
    glBindVertexArray(dummyVAO);
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
    glBindVertexArray(dummyVAO);
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
    glBindVertexArray(dummyVAO);
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
    // vertex tangents
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, tangent));
    // vertex bitangents
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, bitangent));

    // vertex texture coords
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, MAX_BONE_INFLUENCE, GL_INT, sizeof(Vertex),
        (void*)offsetof(Vertex, boneId));

    // vertex texture coords
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_FLOAT, GL_FALSE, sizeof(Vertex),
        (void*)offsetof(Vertex, weight));

    glBindVertexArray(0);
}

void RenderManager::deleteMesh(unsigned int* VAO, unsigned int* VBO, unsigned int* EBO) {
    glDeleteVertexArrays(1, VAO);
    glDeleteBuffers(1, VBO);
    glDeleteBuffers(1, EBO);
}

void RenderManager::setGammaCorrection(float value) {
    this->gammaValue = value;
}
