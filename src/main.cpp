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

#include "fdutil.h"
#include "drawing.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"


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
    inline void operator+=(std::string str) {
        const std::lock_guard<std::mutex> lockg(mux);
        buf += str;
        scrollToBot = true;
    }
    inline const char* c_str() {
        const std::lock_guard<std::mutex> lockg(mux);
        return buf.c_str();
    }
};

void createProj(const std::string& path, LogString& logString) {
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

void buildRunProj(const std::string& activePath, const char* executablePath, LogString& logString) {
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
    std::string activePath("");
    LogString logString;
    std::thread projectThread;

    //init graphics
    initGraphics();

    //init textures
    TextureManager texManager;
    int activeTexture = -1;

    //audio controls
    audio::AudioEngine audioEngine;
    int audioClip = -1;
    std::string audioPath("");
    glm::vec3 audioPos(.0f);

    //asset folder controls
    assetfolder::AssetDescriptor currFolder;
    std::vector<assetfolder::AssetDescriptor> folderItems;
    const float QUERY_INTERVAL = 5.0f;
    float lastQueryTime = float(glfwGetTime());
    float currTime = float(glfwGetTime());
    bool queryFolder = true;

    while (!glfwWindowShouldClose(window)) {
        currTime = float(glfwGetTime());
        if (!queryFolder && (currTime > lastQueryTime + QUERY_INTERVAL)) {
            queryFolder = true;
        }
        // get window dimensions
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        //prepare gui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImVec2 mousePos = ImGui::GetMousePos();

        // --- Get Gui Input ---
        if (ImGui::Begin("Audio Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Active Clip: %s", audioPath.c_str());
            if (ImGui::Button("Load Audio File")) {
                const char* filters[] = { "*.mp3","*.ogg","*.flac","*.wav" };
                const char* filterDesc = "Audio Files";
                std::string path = fdutil::openFile("Select File", NULL,
                    sizeof(filters) / sizeof(filters[0]), filters, filterDesc, false);
                if (!path.empty()) {
                    audio::audioStopAll();
                    audioClip = audio::audioLoad(path.c_str());
                    if (audioClip >= 0) {
                        audioPath = path;
                        logString += "Opened audio file ";
                        logString += path;
                        logString += "\n";
                    }
                    else {
                        logString += "Failed to load audio file ";
                        logString += path;
                        logString += "\n";
                    }
                }
            }

            //adjust source position, listening position is center
            if (ImGui::SliderFloat3("Src Position", &audioPos.x, -1.f, 1.f, "%.3f", 1)) {
                audio::audioSetPosition(audioPos);
            }

            //play audio with 3d effects
            if (ImGui::Button("Play Audio File")) {
                if (!audioPath.empty())
                    audio::audioPlay(audioClip);
            }
            ImGui::End();
        }

        if (ImGui::Begin("Project", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Active Directory: %s", activePath.c_str());
            if (ImGui::Button("Create Project")) {
                std::string path = fdutil::selectFolder("Create a Project", NULL);
                //wait for current op to finish
                if (!path.empty()) {
                    if (projectThread.joinable())
                        projectThread.join();
                    projectThread = std::thread(createProj, path, std::ref(logString));
                    activePath = path;
                    assetfolder::setActiveDirectory(path);
                }
            }
            if (ImGui::Button("Open Project")) {
                std::string path = fdutil::selectFolder("Open Project", NULL);
                if (!path.empty()) {
                    if (projectThread.joinable())
                        projectThread.join();
                    activePath = path;
                    logString += "Opened project at: ";
                    logString += activePath;
                    logString += "\n";
                    assetfolder::setActiveDirectory(path);
                    queryFolder = true;
                }
            }
            if (ImGui::Button("Build and Run")) {
                if (!activePath.empty()) {
                    //wait for current op to finish
                    if (projectThread.joinable())
                        projectThread.join();

                    projectThread = std::thread(buildRunProj, activePath,
                        executablePath, std::ref(logString));

                }
            }
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

        //texture debug
        if (ImGui::Begin("Texture Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::Button("Load Texture")) {
                const char* filters[] = { "*.png","*.jpg","*.bmp","*.tga","*.hdr" };
                std::string path = fdutil::openFile("Load Texture", NULL,
                    sizeof(filters) / sizeof(filters[0]), filters, NULL, 0);

                if (!path.empty()) {
                    clearTextures();
                    activeTexture = loadTexture(path.c_str());
                }
            }
            if (activeTexture != -1) {
                TextureInfo texInfo = getTexture(activeTexture);
                ImGui::Image((void*)texInfo.id, ImVec2(400, 400));
            }

            ImGui::End();
        }

        //folder debug
        ImGui::SetNextWindowSize(ImVec2(600, 200), ImGuiCond_Once);
        if (ImGui::Begin("Folder Browser", NULL, 0)) {
            currFolder = assetfolder::getRootDir();
            ImGui::Text("Current Folder: %s", currFolder.path.c_str());

            if (currFolder.type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
                //import button
                if (ImGui::Button("Import File")) {
                    std::string path = fdutil::openFile("Import File", NULL,
                        0, NULL, NULL, 0);
                    if (!path.empty()) {
                        assetfolder::addAsset(path, currFolder);
                        queryFolder = true;
                    }
                }

                //draw file list
                if (queryFolder) {
                    assetfolder::listDir(currFolder, folderItems);
                    queryFolder = false;
                    lastQueryTime = currTime;
                }
                ImGui::Text("Items: %i", folderItems.size());
                for (unsigned int i = 0;i < folderItems.size();i++) {
                    ImGui::Text("%s %s\n", folderItems[i].name.c_str(),
                        assetfolder::typeToString(folderItems[i].type));
                    ImGui::SameLine();
                    if (ImGui::Button("Delete")) {
                        assetfolder::delAsset(folderItems[i]);
                        queryFolder = true;
                    }
                }
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