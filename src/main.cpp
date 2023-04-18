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
#include "drawing.h"
#include "model_import/model.h"
#include "skeletalmesh_import/animator.h"

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

int main() {
    //switch to correct working directory - platform specific
#ifdef _WIN32
    char executablePath[MAX_PATH];
    int pathLen = GetModuleFileName(NULL, executablePath, MAX_PATH);
    //remove executable name
    for (pathLen--;pathLen >= 0 && executablePath[pathLen] != '\\';pathLen--) {
        executablePath[pathLen] = '\0';
    }
#else
    char executablePath[PATH_MAX];
    int pathLen = readlink("/proc/self/exe", executablePath, PATH_MAX);
    //remove executable name
    for (pathLen--;pathLen >= 0 && executablePath[pathLen] != '/';pathLen--) {
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
    glClearColor(.7f, .7f, .7f, 1.0f);
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
    //init graphics
    initGraphics();

    Model model;
    SkeletalModel skeletalmodel;
    Animator animator;

    while (!glfwWindowShouldClose(window)) {
        // get window dimensions
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        //prepare gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImVec2 mousePos = ImGui::GetMousePos();

        // ImGui::ShowDemoWindow();

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
            ImGui::End();
        }
        if(ImGui::Begin("Model Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Model Loading Demo");
            if (ImGui::Button("Load Model")) {
                const char* path = tinyfd_openFileDialog("Select Model File to Import", NULL, 0, NULL, NULL, 1);
                //wait for current op to finish
                if (path != NULL) {
                    model.loadModel(path);
                }
            }
            ImGui::Text("Model Meshes: %d", model.meshes.size());
            ImGui::Text("Model Textures: %d", model.textures_loaded.size());
            ImGui::Text("From: %s", model.directory.c_str());
            ImGui::End();
        }
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

        if (ImGui::Begin("Skeletal Model Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Skeletal Model Loading Demo");
            if (ImGui::Button("Load Skeletal Model")) {
                const char* path = tinyfd_openFileDialog("Select a Skeletal Model File to Import", NULL, 0, NULL, NULL, 1);
                //wait for current op to finish
                if (path != NULL) {
                    skeletalmodel.loadModel(path);
                }
            }
            ImGui::Text("Model Meshes: %d", skeletalmodel.meshes.size());
            ImGui::Text("Model Textures: %d", skeletalmodel.textures_loaded.size());
            ImGui::Text("From: %s", skeletalmodel.directory.c_str());
            ImGui::Text("Bones: %d", skeletalmodel.boneCounter);
            ImGui::End();
        }

        ImGui::SetNextWindowContentSize(ImVec2(800, 0.0f));
        if (ImGui::Begin("Virtual Keys", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button("Add Animation")) {
                const char* path = tinyfd_openFileDialog("Select Animation to Import", NULL, 0, NULL, NULL, 1);
                //wait for current op to finish
                if (path != NULL) {
                    animator.loadAnimation(path, &skeletalmodel);
                }
            }
            ImGui::Text("Animations:");
            ImGui::Columns(2);
            ImGui::Separator();
            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::Text("Duration");
            ImGui::NextColumn();
            ImGui::Separator();
            for (unsigned int i = 0; i < animator.animations.size(); i++) {
                ImGui::PushID(i);
                ImGui::InputText("##anim_name", &animator.animations[i].name[0], animator.animations[i].name.capacity());
                animator.animations[i].name.resize(std::strlen(&animator.animations[i].name[0]));
                ImGui::NextColumn();
                ImGui::Text("%f", animator.animations[i].duration);
                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::Columns(1);
            if (ImGui::Button("Add Node")) {
                animator.addNode();
            }
            ImGui::Text("Nodes:");
            ImGui::Columns(5);
            ImGui::Separator();
            ImGui::Text("Name");
            ImGui::NextColumn();
            ImGui::Text("Animation");
            ImGui::NextColumn();
            ImGui::Text("Loop Count");
            ImGui::NextColumn();
            ImGui::Text("Select Node");
            ImGui::NextColumn();
            ImGui::Text("Remove Node");
            ImGui::NextColumn();
            ImGui::Separator();
            for (unsigned int i = 0; i < animator.nodes.size(); i++) {
                ImGui::PushID(i);
                ImGui::InputText("##node_name", &animator.nodes[i].name[0], animator.nodes[i].name.capacity());
                animator.nodes[i].name.resize(std::strlen(&animator.nodes[i].name[0]));
                ImGui::NextColumn();
                if (ImGui::BeginCombo("##animation", animator.nodes[i].animation->name.c_str())) {
                    for (unsigned int j = 0; j < animator.animations.size(); j++) {
                        if (ImGui::Selectable(animator.animations[j].name.c_str(), (animator.nodes[i].animation == &animator.animations[j]))) {
                            animator.nodes[i].animation = &animator.animations[j];
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::NextColumn();
                ImGui::InputScalar("##loop_count", ImGuiDataType_U32 , &animator.nodes[i].loopCount);
                ImGui::NextColumn();
                if (ImGui::Button("Select Node")) {
                    animator.selectedNode = &animator.nodes[i];
                }
                ImGui::NextColumn();
                if (ImGui::Button("Remove Node")) {
                    animator.removeNode(i);
                }
                ImGui::NextColumn();
                ImGui::PopID();
            }
            ImGui::Columns(1);
            if (ImGui::Button("Add No Condition Transition")) {
                animator.addNoConditionTransition();
            }
            if (ImGui::Button("Add Boolean Transition")) {
                animator.addBoolACTransition();
            }
            if (ImGui::Button("Add Integer Transition")) {
                animator.addIntACTransition();
            }
            if (ImGui::Button("Add Float Transition")) {
                animator.addFloatACTransition();
            }
            ImGui::Text("Transitions:");
            if (animator.selectedNode) {
                ImGui::Columns(2);
                ImGui::Separator();
                ImGui::Text("Destination Node");
                ImGui::NextColumn();
                ImGui::Text("Remove Transition");
                ImGui::NextColumn();
                ImGui::Separator();
                for (unsigned int i = 0; i < animator.selectedNode->noConditionTransitions.size(); i++) {
                    ImGui::PushID(i);
                    if (ImGui::BeginCombo("##transition_to_nocond", animator.selectedNode->noConditionTransitions[i].transitionTo->name.c_str())) {
                        for (unsigned int j = 0; j < animator.nodes.size(); j++) {
                            if (ImGui::Selectable(animator.nodes[j].name.c_str(), animator.selectedNode->noConditionTransitions[i].transitionTo == &animator.nodes[j])) {
                                animator.selectedNode->noConditionTransitions[i].transitionTo = &animator.nodes[j];
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::NextColumn();
                    if (ImGui::Button("Remove Transition")) {
                        animator.removeNoConditionTransition(i);
                    }
                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns(5);
                ImGui::Separator();
                ImGui::Text("Destination Node");
                ImGui::NextColumn();
                ImGui::Text("Transition Immediately?");
                ImGui::NextColumn();
                ImGui::Text("Current Value");
                ImGui::NextColumn();
                ImGui::Text("Desired Value");
                ImGui::NextColumn();
                ImGui::Text("Remove Transition");
                ImGui::NextColumn();
                ImGui::Separator();
                for (unsigned int i = 0; i < animator.selectedNode->boolTransitions.size(); i++) {
                    ImGui::PushID(i);
                    if (ImGui::BeginCombo("##transition_to_bool", animator.selectedNode->boolTransitions[i].transitionTo->name.c_str())) {
                        for (unsigned int j = 0; j < animator.nodes.size(); j++) {
                            if (ImGui::Selectable(animator.nodes[j].name.c_str(), animator.selectedNode->boolTransitions[i].transitionTo == &animator.nodes[j])) {
                                animator.selectedNode->boolTransitions[i].transitionTo = &animator.nodes[j];
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::NextColumn();
                    ImGui::Checkbox("##immediate_bool", &animator.selectedNode->boolTransitions[i].immediate);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##condition_bool", &animator.selectedNode->boolTransitions[i].condition);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##desired_bool", &animator.selectedNode->boolTransitions[i].desiredValue);
                    ImGui::NextColumn();
                    if (ImGui::Button("Remove Transition")) {
                        animator.removeBoolACTransition(i);
                    }
                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns(8);
                ImGui::Separator();
                ImGui::Text("Destination Node");
                ImGui::NextColumn();
                ImGui::Text("Transition Immediately?");
                ImGui::NextColumn();
                ImGui::Text("Current Value");
                ImGui::NextColumn();
                ImGui::Text("Desired Value");
                ImGui::NextColumn();
                ImGui::Text("Condition Lower?");
                ImGui::NextColumn();
                ImGui::Text("Condition Equal?");
                ImGui::NextColumn();
                ImGui::Text("Condition Greater?");
                ImGui::NextColumn();
                ImGui::Text("Remove Transition");
                ImGui::NextColumn();
                ImGui::Separator();
                for (unsigned int i = 0; i < animator.selectedNode->intTransitions.size(); i++) {
                    ImGui::PushID(i);
                    if (ImGui::BeginCombo("##transition_to_int", animator.selectedNode->intTransitions[i].transitionTo->name.c_str())) {
                        for (unsigned int j = 0; j < animator.nodes.size(); j++) {
                            if (ImGui::Selectable(animator.nodes[j].name.c_str(), animator.selectedNode->intTransitions[i].transitionTo == &animator.nodes[j])) {
                                animator.selectedNode->intTransitions[i].transitionTo = &animator.nodes[j];
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::NextColumn();
                    ImGui::Checkbox("##immediate_int", &animator.selectedNode->intTransitions[i].immediate);
                    ImGui::NextColumn();
                    ImGui::Text("%d", animator.selectedNode->intTransitions[i].condition);
                    ImGui::NextColumn();
                    ImGui::InputInt("##desired_int", &animator.selectedNode->intTransitions[i].desiredValue);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##should_lower_int", &animator.selectedNode->intTransitions[i].shouldBeLower);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##should_equal_int", &animator.selectedNode->intTransitions[i].shouldBeEqual);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##should_greater_int", &animator.selectedNode->intTransitions[i].shouldBeGreater);
                    ImGui::NextColumn();
                    if (ImGui::Button("Remove Transition")) {
                        animator.removeIntACTransition(i);
                    }
                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns(8);
                ImGui::Separator();
                ImGui::Text("Destination Node");
                ImGui::NextColumn();
                ImGui::Text("Transition Immediately?");
                ImGui::NextColumn();
                ImGui::Text("Current Value");
                ImGui::NextColumn();
                ImGui::Text("Desired Value");
                ImGui::NextColumn();
                ImGui::Text("Condition Lower?");
                ImGui::NextColumn();
                ImGui::Text("Condition Equal?");
                ImGui::NextColumn();
                ImGui::Text("Condition Greater?");
                ImGui::NextColumn();
                ImGui::Text("Remove Transition");
                ImGui::NextColumn();
                ImGui::Separator();
                for (unsigned int i = 0; i < animator.selectedNode->floatTransitions.size(); i++) {
                    ImGui::PushID(i);
                    if (ImGui::BeginCombo("##transition_to_float", animator.selectedNode->floatTransitions[i].transitionTo->name.c_str())) {
                        for (unsigned int j = 0; j < animator.nodes.size(); j++) {
                            if (ImGui::Selectable(animator.nodes[j].name.c_str(), animator.selectedNode->floatTransitions[i].transitionTo == &animator.nodes[j])) {
                                animator.selectedNode->floatTransitions[i].transitionTo = &animator.nodes[j];
                            }
                        }
                        ImGui::EndCombo();
                    }
                    ImGui::NextColumn();
                    ImGui::Checkbox("##immediate_float", &animator.selectedNode->floatTransitions[i].immediate);
                    ImGui::NextColumn();
                    ImGui::Text("%f", animator.selectedNode->floatTransitions[i].condition);
                    ImGui::NextColumn();
                    ImGui::InputFloat("##desired_float", &animator.selectedNode->floatTransitions[i].desiredValue);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##should_lower_float", &animator.selectedNode->floatTransitions[i].shouldBeLower);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##should_equal_float", &animator.selectedNode->floatTransitions[i].shouldBeEqual);
                    ImGui::NextColumn();
                    ImGui::Checkbox("##should_greater_float", &animator.selectedNode->floatTransitions[i].shouldBeGreater);
                    ImGui::NextColumn();
                    if (ImGui::Button("Remove Transition")) {
                        animator.removeFloatACTransition(i);
                    }
                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns(1);
            }
            else {
                ImGui::Text("Select a node to show it's transitions");
            }
            ImGui::End();
        }

        //--- Draw Results ---
        //adapt to resize
        glViewport(0, 0, width, height);

        //prepare gui for rendering
        ImGui::Render();

        //draw background
        glClearColor(.7f, .7f, .7f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    if (projectThread.joinable())
        projectThread.join();

    return 0;
}