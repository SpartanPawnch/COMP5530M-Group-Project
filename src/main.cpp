#ifdef _WIN32
#include<windows.h>
#include<direct.h>
#else
#include<unistd.h>
#include<limits.h>
#endif

#include <iostream>
#include <stdio.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <thread>
#include <mutex>

#include "../external/tinyfiledialogs/tinyfiledialogs.h"
#include "../render-engine/RenderManager.h"
#include "drawing.h"

// Define the cube's vertices positions and colors separately
GLfloat cubePositions[] = {
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f
};


GLfloat cubeColors[] = {
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f
};


// Define the cube's vertex indices
GLuint cubeIndices[] = {
    0, 1, 2, 2, 3, 0,    // Front face
    1, 5, 6, 6, 2, 1,    // Right face
    4, 0, 3, 3, 7, 4,    // Left face
    5, 4, 7, 7, 6, 5,    // Back face
    3, 2, 6, 6, 7, 3,    // Top face
    4, 5, 1, 1, 0, 4     // Bottom face
};

RenderManager* renderManager;

struct LogString {
private:
    std::string buf;
    std::mutex mux;
public:
    bool scrollToBot = false;
    inline void operator+=(const char* str) {
        const std::lock_guard<std::mutex> lockg(mux);
        buf += str;
        scrollToBot = true;
    }
    inline const char* c_str() {
        const std::lock_guard<std::mutex> lockg(mux);
        return buf.c_str();
    }
};

void createProj(const char* path, LogString& logString) {
    char buf[1024];
    FILE* copyProc = _popen((std::string("xcopy /s /e /q /y .\\template ") + path).c_str(), "r");
    while (!feof(copyProc)) {
        fgets(buf, sizeof(char) * 1024, copyProc);
        logString += buf;
    }
    logString += "Created project at ";
    logString += path;
    logString += "\n";
    fclose(copyProc);
}

void buildRunProj(const char* activePath, const char* executablePath, LogString& logString) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    //change to buildDir
    std::string buildDir = std::string(activePath) + "/build";
    _mkdir(buildDir.c_str());
    _chdir(buildDir.c_str());

    //build target
    FILE* cmakeProc = _popen("cmake .. && cmake --build . --target BuildTest", "r");
    char buf[1024];
    while (!feof(cmakeProc)) {
        fgets(buf, sizeof(char) * 1024, cmakeProc);
        logString += buf;
    }
    logString += "Building Done!\n";

    //build cleanup
    fclose(cmakeProc);

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(NULL,   // No module name (use command line)
        "./Debug/BuildTest.exe",        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi);           // Pointer to PROCESS_INFORMATION structure

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    //restore path
    _chdir(executablePath);
}

void handleKeyboardInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        // Move the camera forward
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        // Move the camera backward
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        // Strafe the camera left
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        // Strafe the camera right
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 3);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        // Ascend camera
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 4);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
    {
        // Descend camera
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 5);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        // Reset camera position
        renderManager->camera->resetPosition();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        //increase camera movement speed
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 6);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        //increase camera movement speed
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 7);
    }
 
}

void handleMouseInput(GLFWwindow* window)
{
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (renderManager->camera->focusState == false)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderManager->camera->focusState = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderManager->camera->focusState = false;
        }

        if (renderManager->firstRClick == true)
        {
            renderManager->xPosLast = renderManager->xPos;
            renderManager->yPosLast = renderManager->yPos;
            renderManager->firstRClick = false;
        }
    }
    
    if (renderManager->camera->focusState == true)
    {
        //now we can change the orientation of the camera
        glfwGetCursorPos(window, &renderManager->xPos, &renderManager->yPos);

        //offset
        renderManager->xOffset = renderManager->xPos - renderManager->xPosLast;
        renderManager->yOffset = renderManager->yPos - renderManager->yPosLast;

        //send data to camera
        renderManager->camera->updateInput(renderManager->deltaTime,
            -1, renderManager->xOffset, renderManager->yOffset);

        renderManager->xPosLast = renderManager->xPos;
        renderManager->yPosLast = renderManager->yPos;
        glfwSetCursorPos(window, renderManager->xPosLast, renderManager->yPosLast);
    }
    
}
//set renderEngine instance to nullptr initially
RenderManager* RenderManager::instance = nullptr;

int main() {
    //switch to correct working directory - platform specific
#ifdef _WIN32
    char executablePath[MAX_PATH];
    int pathLen = GetModuleFileName(NULL, executablePath, MAX_PATH);
    //remove executable name
    for (pathLen--; pathLen >= 0 && executablePath[pathLen] != '\\'; pathLen--) {
        executablePath[pathLen] = '\0';
    }
#else
    char executablePath[PATH_MAX];
    int pathLen = readlink("/proc/self/exe", executablePath, PATH_MAX);
    //remove executable name
    for (pathLen--; pathLen >= 0 && executablePath[pathLen] != '/'; pathLen--) {
        executablePath[pathLen] = '\0';
    }
#endif

    _chdir(executablePath);

    //init glfw and setup window
    GLFWwindow* window;
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //create window
    window = glfwCreateWindow(1600, 900, "Build System Test", NULL, NULL);
    glfwMakeContextCurrent(window);

    //setup OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(err) << "\n";
        return 1;
    }

    //swap buffers to avoid the window not responding
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    //Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    const char* activePath = NULL;
    LogString logString;
    std::thread projectThread;
  
    //setup components
    //Render Engine
    renderManager = RenderManager::getInstance();

    renderManager->startUp(window);

    //init shader
    const char* vertexPath = "../../../assets/shaders/colours.vert";
    const char* fragPath = "../../../assets/shaders/colours.frag";
 //   RenderPipeline* program = new RenderPipeline(vertexPath, fragPath);
    renderManager->addPipeline(vertexPath, fragPath);

    //vertex array object (VAO)
    //GLuint VAO;
    //glGenVertexArrays(1, &VAO);
    //glBindVertexArray(VAO);

    //create vertex buffer object(VBO)
    VertexBuffer posVBO(sizeof(cubePositions), cubePositions, PositionsBuffer);
    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
   // glEnableVertexAttribArray(0);

    VertexBuffer colVBO(sizeof(cubeColors), cubeColors, ColorsBuffer);
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //glEnableVertexAttribArray(1);

    //create an element buffer object for the indices
    IndexBuffer EBO(sizeof(cubeIndices), cubeIndices);
 
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    double previous_time = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        // get window dimensions
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        double current_time = glfwGetTime();
        renderManager->deltaTime = current_time - previous_time;
        
        //handle inputs
        handleKeyboardInput(window);
        handleMouseInput(window);
        previous_time = current_time;

        //update matrices
        renderManager->updateMatrices(&width, &height);

        //prepare gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImVec2 mousePos = ImGui::GetMousePos();

        // --- Get Gui Input ---
        if (ImGui::Begin("Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Active Directory: %s", activePath);
            if (ImGui::Button("Create Project")) {
                const char* path = tinyfd_selectFolderDialog("Create a Project", NULL);
                //wait for current op to finish
                if (path != NULL) {
                    if (projectThread.joinable())
                        projectThread.join();
                    projectThread = std::thread(createProj, path, std::ref(logString));
                    activePath = path;
                }
            }
            if (ImGui::Button("Open Project")) {
                const char* path = tinyfd_selectFolderDialog("Create a Project", NULL);
                if (path != NULL) {
                    if (projectThread.joinable())
                        projectThread.join();
                    activePath = path;
                    logString += "Opened project at: ";
                    logString += activePath;
                    logString += "\n";
                }
            }
            if (ImGui::Button("Build and Run")) {
                if (activePath != NULL) {
                    //wait for current op to finish
                    if (projectThread.joinable())
                        projectThread.join();

                    projectThread = std::thread(buildRunProj, activePath,
                        executablePath, std::ref(logString));

                }
            }
            
        }
        ImGui::End();
        ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_Always);
        if (ImGui::Begin("Log", NULL, NULL)) {
            ImGui::PushTextWrapPos(560);
            ImGui::TextUnformatted(logString.c_str());
            ImGui::PopTextWrapPos();
            if (logString.scrollToBot) {
                logString.scrollToBot = false;
                ImGui::SetScrollHereY(1.0f);
            }
            ImGui::End();
        }

        //--- Draw Results ---
        //adapt to resize
        glViewport(0, 0, width, height);

        //prepare gui for rendering
        ImGui::Render();

        //draw background
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //RENDERING
        glUseProgram(renderManager->getPipeline(0)->getProgram());

        ////handle for uniforms
        GLuint ModelID = glGetUniformLocation(renderManager->getPipeline(0)->getProgram(), "model");
        GLuint ViewID = glGetUniformLocation(renderManager->getPipeline(0)->getProgram(), "view");
        GLuint ProjectionID = glGetUniformLocation(renderManager->getPipeline(0)->getProgram(), "projection");

        glUniformMatrix4fv(ModelID, 1, GL_FALSE, &renderManager->modelMatrix[0][0]);
        glUniformMatrix4fv(ViewID, 1, GL_FALSE, &renderManager->viewMatrix[0][0]);
        glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, &renderManager->projectionMatrix[0][0]);

        //// Render the cube
        glBindVertexArray(renderManager->getPipeline(0)->getVAO());
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    if (projectThread.joinable())
        projectThread.join();

    return 0;
}