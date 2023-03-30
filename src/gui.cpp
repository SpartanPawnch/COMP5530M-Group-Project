#include "gui.h"

#ifdef _WIN32
#include<windows.h>
#include<direct.h>
#else
#include<unistd.h>
#include<limits.h>
#endif

#include <iostream>
#include <stdio.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <thread>
#include <mutex>

#include "fdutil.h"
#include "drawing.h"
#include "logging.h"
#include "levels.h"
#include "gui.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "model_import/model.h"

using namespace ImGui;

// --- GUI constants, possibly replace with settings file ---
namespace guicfg {
    const ImVec2 assetMgrPadding(10.0f, 25.0f);
    const ImVec2 assetMgrIconSize(60.0f, 60.0f);
    const ImVec2 assetMgrItemSize(80.0f, 80.0f);
    const float splitterThickness = 7.f;
    const ImGuiID dockspaceID = 1;
};

void createProj(const std::string& path) {
    char buf[1024];
    FILE* copyProc = _popen((std::string("xcopy /s /e /q /y .\\template ") + path).c_str(), "r");
    while (!feof(copyProc)) {
        fgets(buf, sizeof(char) * 1024, copyProc);
        logging::logInfo(buf);
    }
    logging::logInfo("Created project at {}\n", path);
    fclose(copyProc);
}

void buildRunProj(const std::string& activePath, const char* executablePath) {
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
        logging::logInfo(buf);
    }
    logging::logInfo("Building Done!\n");

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

//utility function - set all elements to specific value
template<typename T>
void setVec(std::vector<T>& vec, T val) {
    for (unsigned int i = 0;i < vec.size();i++) {
        vec[i] = val;
    }
}


static std::string activePath;

static std::thread projectThread;

static int activeTexture = -1;

static int audioClip = -1;
static std::string audioPath("");
static glm::vec3 audioPos(.0f);

//asset manager controls
static assetfolder::AssetDescriptor currFolder = { "","",assetfolder::AssetDescriptor::EFileType::INVALID };
static std::vector<assetfolder::AssetDescriptor> folderItems;
static const float QUERY_INTERVAL = 5.0f;
static bool queryFolder = true;

static int fileTexture;
static int folderTexture;

static Model model;

GLuint viewportFramebuffer;
static GLuint viewportTex;

GUIManager::GUIManager() {
    fileTexture = loadTexture("assets/fileico.png");
    folderTexture = loadTexture("assets/folderico.png");

    //create viewport framebuffer
    glGenFramebuffers(1, &viewportFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportFramebuffer);

    glGenTextures(1, &viewportTex);
    glBindTexture(GL_TEXTURE_2D, viewportTex);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, viewportTex, 0);
}
GUIManager::~GUIManager() {
    glDeleteFramebuffers(1, &viewportFramebuffer);
    glDeleteTextures(1, &viewportTex);
    if (projectThread.joinable())
        projectThread.join();
}

inline void drawAudioDemo() {
    if (ImGui::Begin("Audio Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Active Clip: %s", audioPath.c_str());
        if (ImGui::Button("Load Audio File")) {
            const char* filters[] = { "*.mp3","*.ogg","*.flac","*.wav" };
            const char* filterDesc = "Audio Files";
            std::string path = fdutil::openFile("Select File", NULL,
                sizeof(filters) / sizeof(filters[0]), filters, filterDesc);
            if (!path.empty()) {
                audio::audioStopAll();
                audioClip = audio::audioLoad(path.c_str());
                if (audioClip >= 0) {
                    audioPath = path;
                    logging::logInfo("Opened audio file {}\n", path);
                }
                else {
                    logging::logErr("Failed to load audio file ", path);
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
    }
    ImGui::End();
}

inline void drawProjectWindow(const char* executablePath) {
    //TODO (Jub) - switch to main menu options
    if (ImGui::Begin("Project", NULL)) {
        ImGui::Text("Active Directory: %s", activePath.c_str());
        if (ImGui::Button("Create Project")) {
            std::string path = fdutil::selectFolder("Create a Project", NULL);
            //wait for current op to finish
            if (!path.empty()) {
                if (projectThread.joinable())
                    projectThread.join();
                projectThread = std::thread(createProj, path);
                activePath = path;
                assetfolder::setActiveDirectory(path);
                currFolder = assetfolder::getRootDir();
                queryFolder = true;
            }
        }
        if (ImGui::Button("Open Project")) {
            std::string path = fdutil::selectFolder("Open Project", NULL);
            if (!path.empty()) {
                if (projectThread.joinable())
                    projectThread.join();
                activePath = path;
                logging::logInfo("Opened project at: {}\n", activePath.c_str());
                assetfolder::setActiveDirectory(path);
                currFolder = assetfolder::getRootDir();
                queryFolder = true;
            }
        }
        if (ImGui::Button("Build and Run")) {
            if (!activePath.empty()) {
                //wait for current op to finish
                if (projectThread.joinable())
                    projectThread.join();

                projectThread = std::thread(buildRunProj, activePath,
                    executablePath);

            }
        }
    }
    ImGui::End();
}

inline void drawModelDemo() {
    if (ImGui::Begin("Model Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("Model Loading Demo");
        if (ImGui::Button("Load Model")) {
            std::string path = fdutil::openFile("Select Model File to Import", NULL, 0, NULL, NULL);
            //wait for current op to finish
            if (!path.empty()) {
                model.loadModel(path);
            }
        }
        ImGui::Text("Model Meshes: %d", model.meshes.size());
        ImGui::Text("Model Textures: %d", model.textures_loaded.size());
        ImGui::Text("From: %s", model.directory.c_str());
    }
    ImGui::End();
}

inline void drawLog() {
    if (ImGui::Begin("Log", NULL, NULL)) {
        ImGui::PushTextWrapPos(ImGui::GetWindowSize().x - 40.0f);
        ImGui::TextUnformatted(logging::getLogString());
        ImGui::PopTextWrapPos();
        if (logging::scrollToBot) {
            logging::scrollToBot = false;
            ImGui::SetScrollHereY(1.0f);
        }
    }
    ImGui::End();
}

inline void drawTextureDebug() {
    // texture debug
    if (ImGui::Begin("Texture Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (ImGui::Button("Load Texture")) {
            const char* filters[] = { "*.png","*.jpg","*.bmp","*.tga","*.hdr" };
            std::string path = fdutil::openFile("Load Texture", NULL,
                sizeof(filters) / sizeof(filters[0]), filters, NULL);

            if (!path.empty()) {
                clearTextures();
                activeTexture = loadTexture(path.c_str());
            }
        }
        if (activeTexture != -1) {
            TextureInfo texInfo = getTexture(activeTexture);
            ImGui::Image((void*)texInfo.id, ImVec2(400, 400));
        }

    }
    ImGui::End();
}

inline void drawAssetBrowser(GLFWwindow* window) {
    //folder debug
    if (ImGui::Begin("Asset Browser", NULL)) {
        ImGui::Text("Current Folder: %s", currFolder.path.c_str());

        if (currFolder.type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
            static std::vector<bool> itemIsSelected;
            int itemsPerLine = int(ImGui::GetWindowSize().x / (guicfg::assetMgrItemSize.x +
                guicfg::assetMgrPadding.x));

            //refresh folder if needed
            if (queryFolder) {
                assetfolder::listDir(currFolder, folderItems);
                queryFolder = false;
                itemIsSelected.resize(folderItems.size(), false);
            }

            //import buttons
            if (ImGui::Button("Import File")) {
                std::vector<std::string> paths;
                fdutil::openFileMulti("Import File", NULL,
                    0, NULL, NULL, paths);
                if (!paths.empty()) {
                    assetfolder::addAssets(paths, currFolder);
                    queryFolder = true;
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Import Folder")) {
                std::string path = fdutil::selectFolder("Import Folder", NULL);
                if (!path.empty()) {
                    assetfolder::addAsset(path, currFolder);
                    queryFolder = true;
                }
            }

            //delete button
            ImGui::SameLine();
            if (ImGui::Button("Delete") || glfwGetKey(window, GLFW_KEY_DELETE) == GLFW_PRESS) {
                for (unsigned int i = 0;i < itemIsSelected.size();i++) {
                    if (itemIsSelected[i])
                        assetfolder::delAsset(folderItems[i]);
                }
                setVec(itemIsSelected, false);
                queryFolder = true;
            }

            //outer dir shorcut
            ImGui::SameLine();
            if (ImGui::Button("Root")) {
                currFolder = assetfolder::getRootDir();
                setVec(itemIsSelected, false);
                queryFolder = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("..")) {
                currFolder = assetfolder::outerDir(currFolder);
                setVec(itemIsSelected, false);
                queryFolder = true;
            }

            //refresh button
            ImGui::SameLine();
            if (ImGui::Button("Refresh")) {
                queryFolder = true;
            }

            //draw files in folder
            ImVec2 winPos = ImGui::GetWindowPos();
            float initialItemPad = ImGui::GetCursorPosX();
            for (unsigned int i = 0;i < folderItems.size();i++) {
                ImGui::PushID(i);
                ImVec2 initialPos = ImGui::GetCursorPos();
                ImGui::BeginGroup();
                //icon
                if (folderItems[i].type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
                    TextureInfo texInfo = getTexture(folderTexture);
                    ImGui::Image((void*)texInfo.id, guicfg::assetMgrIconSize);
                }
                else {
                    TextureInfo texInfo = getTexture(fileTexture);
                    ImGui::Image((void*)texInfo.id, guicfg::assetMgrIconSize);
                }

                //filename
                ImGui::PushClipRect(ImVec2(winPos.x + initialPos.x, winPos.y + initialPos.y),
                    ImVec2(winPos.x + initialPos.x + guicfg::assetMgrItemSize.x,
                        winPos.y + initialPos.y + guicfg::assetMgrItemSize.y), false);
                ImGui::Text("%s", folderItems[i].name.c_str());
                ImGui::PopClipRect();

                //selectable
                ImGui::SetCursorPos(initialPos);
                if (ImGui::Selectable("##fileselector", itemIsSelected[i], 0, guicfg::assetMgrItemSize)) {
                    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL)) {
                        itemIsSelected[i] = true;
                    }
                    else {
                        setVec(itemIsSelected, false);
                        itemIsSelected[i] = true;
                    }
                }

                ImGui::EndGroup();

                if (ImGui::IsItemHovered()) {
                    //check for folder switch
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) &&
                        folderItems[i].type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
                        currFolder = folderItems[i];
                        setVec(itemIsSelected, false);
                        queryFolder = true;
                    }

                    //draw tooltip - full path
                    ImGui::SetTooltip(folderItems[i].path.c_str());
                }

                if (i < folderItems.size() - 1) {
                    //wrap to next line if needed
                    bool wrap = i % itemsPerLine == itemsPerLine - 1;
                    ImVec2 nextPos = ImVec2(
                        wrap ? initialItemPad : (initialPos.x +
                            guicfg::assetMgrItemSize.x + guicfg::assetMgrPadding.x),
                        initialPos.y + (guicfg::assetMgrIconSize.y + guicfg::assetMgrPadding.y) *
                        wrap
                    );
                    ImGui::SetCursorPos(
                        nextPos);
                }

                ImGui::PopID();
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
                setVec(itemIsSelected, false);
            }
        }

    }
    ImGui::End();
}
int viewportTexWidth = 0;
int viewportTexHeight = 0;

void drawViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
    if (ImGui::Begin("Viewport")) {
        //adjust for titlebar
        ImVec2 windowSize = ImGui::GetWindowSize();
        windowSize.y -= ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;

        viewportTexWidth = int(windowSize.x);
        viewportTexHeight = int(windowSize.y);

        //adjust to window resize
        glBindTexture(GL_TEXTURE_2D, viewportTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportTexWidth,
            viewportTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        ImGui::Image((void*)viewportTex, windowSize);
    }
    ImGui::PopStyleVar(2);
    ImGui::End();
}

void prepUI(GLFWwindow* window, const char* executablePath, float dt,
    int viewportWidth, int viewportHeight) {
    ImVec2 windowSize = ImVec2(float(viewportWidth), float(viewportHeight));
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 mousePos = ImGui::GetMousePos();

    static ImGuiID dockCenter = 0;
    static ImGuiID dockLeft;
    static ImGuiID dockRight;
    static ImGuiID dockBot;

    //create layout if not present already
    ImGui::SetNextWindowPos(ImVec2(.0f, .0f));
    ImGui::SetNextWindowSize(windowSize);
    if (ImGui::Begin("##FullscreenWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {

        ImGuiID dockId = ImGui::GetID("DockspaceDefault");
        ImGui::DockSpace(dockId);
        static bool dockSpaceInit = false;
        if (!dockSpaceInit) {
            //create initial empty node
            ImGui::DockBuilderRemoveNode(dockId);
            dockCenter = ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockCenter, windowSize);
            ImGui::DockBuilderSetNodePos(dockCenter, ImGui::GetMainViewport()->Pos);

            //TODO dynamic layout
            //split vertically
            dockBot = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, .4f, NULL, &dockCenter);

            //split horizontally twice
            dockLeft = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left, .25f, NULL, &dockCenter);
            dockRight = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, .25f, NULL, &dockCenter);

            ImGui::DockBuilderFinish(dockId);
            dockSpaceInit = true;
        }
    }
    ImGui::End();

    //TODO fix crash on docking multiple windows in same node
    //TODO undocked windows in front

    ImGui::SetNextWindowDockID(dockLeft, ImGuiCond_Once);
    drawProjectWindow(executablePath);
    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Once);
    drawLog();
    ImGui::SetNextWindowDockID(dockBot, ImGuiCond_Once);
    drawAssetBrowser(window);
    ImGui::SetNextWindowDockID(dockCenter, ImGuiCond_Once);
    drawViewport();

    //prepare gui for rendering
    ImGui::Render();
}

void drawUI() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}