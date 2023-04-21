#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#else
#include <unistd.h>
#include <limits.h>
#endif

#pragma warning(disable : 4312)
#pragma warning(disable : 4244)

#include <iostream>
#include <stdio.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>
#include <glm/common.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <thread>
#include <mutex>

#include "fdutil.h"
#include "logging.h"
#include "levels.h"
#include "gui.h"
#include "util.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "model_import/model.h"
#include "ECS/Component/AudioSourceComponent.h"
#include "ECS/Entity/CameraEntity.h"
#include "ECS/Entity/ModelEntity.h"
#include "ECS/Entity/SkeletalMeshEntity.h"
#include "ECS/Scene/Scene.h"
#include "../render-engine/RenderManager.h"

using namespace ImGui;

// --- GUI constants, possibly replace with settings file ---
namespace guicfg {
    const ImVec2 assetMgrPadding(10.0f, 25.0f);
    const ImVec2 assetMgrIconSize(60.0f, 60.0f);
    const ImVec2 assetMgrItemSize(80.0f, 80.0f);

    const char* titleFontPath = "fonts/Phudu-Medium.ttf";
    const char* regularFontPath = "fonts/Armata-Regular.ttf";

    ImFont* titleFont = nullptr;
    ImFont* regularFont = nullptr;
};

// --- Build System Utilities ---
void createProj(const std::string& path) {
    char buf[1024];
    FILE* copyProc =
        _popen((std::string("xcopy /s /e /q /y .\\template \"") + path + "\" 2>>&1").c_str(), "r");
    while (!feof(copyProc)) {
        fgets(buf, sizeof(char) * 1024, copyProc);
        logging::logInfo(buf);
    }
    int res = _pclose(copyProc);
    if (res == 0) {
        logging::logInfo("Created project at {}\n", path);
        _mkdir((path + "/assets").c_str());
    }
}

void buildRunProj(const std::string& activePath, const char* executablePath) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    // change to buildDir
    std::string buildDir = std::string(activePath) + "/build";
    _mkdir(buildDir.c_str());
    _chdir(buildDir.c_str());

    // build target
    FILE* cmakeProc =
        _popen("cmake .. 2>>&1 && cmake --build . -j 24 --target BuildTest 2>>&1", "r");
    char buf[1024];
    while (!feof(cmakeProc)) {
        fgets(buf, sizeof(char) * 1024, cmakeProc);
        logging::logInfo(buf);
    }
    logging::logInfo("Building Done!\n");

    // build cleanup
    int res = _pclose(cmakeProc);

    if (res != 0) {
        logging::logErr("Build returned error {}\n\n", res);
        return;
    }
    logging::logInfo("Build returned 0\n\n");

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(NULL, // No module name (use command line)
        LPSTR("./Debug/BuildTest.exe"), // Command line
        NULL, // Process handle not inheritable
        NULL, // Thread handle not inheritable
        FALSE, // Set handle inheritance to FALSE
        0, // No creation flags
        NULL, // Use parent's environment block
        NULL, // Use parent's starting directory
        &si, // Pointer to STARTUPINFO structure
        &pi); // Pointer to PROCESS_INFORMATION structure

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    // restore path
    _chdir(executablePath);
}

// --- Internal GUI State ---

// base functionality vars
static GLFWwindow* baseWindow;
static std::string activePath;
static std::thread projectThread;

// texture demo vars
static int activeTexture = -1;

// audio demo vars
static int audioClip = -1;
static std::string audioPath("");
static glm::vec3 audioPos(.0f);

// asset manager controls
static assetfolder::AssetDescriptor currAssetFolder = {
    "", "", assetfolder::AssetDescriptor::EFileType::INVALID};
static std::vector<assetfolder::AssetDescriptor> folderItems;
static bool queryAssetsFolder = true;
static bool queryLevelsFolder = true;

// ui textures
static int fileTexture;
static int folderTexture;

// model demo vars
static Model model;

// renderer vars
static RenderManager* renderManager;

// editor vars
Scene scene;

// viewport widget vars
GLuint viewportFramebuffer;
static GLuint viewportTex;
static GLuint viewportDepthBuf;

// TODO Load/Save style to disk
static ImGuiStyle guiStyle;

// --- Module Init/Deinit
GUIManager::GUIManager(GLFWwindow* window) {
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    // enable docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // load fonts
    ImFont* defaultFont = io.Fonts->AddFontDefault();
    guicfg::titleFont = io.Fonts->AddFontFromFileTTF(guicfg::titleFontPath, 16.f);
    guicfg::regularFont = io.Fonts->AddFontFromFileTTF(guicfg::regularFontPath, 15.f);
    io.Fonts->Build();

    // fallback to default if loading fails
    guicfg::titleFont = guicfg::titleFont ? guicfg::titleFont : defaultFont;
    guicfg::regularFont = guicfg::regularFont ? guicfg::regularFont : defaultFont;

    io.FontDefault = guicfg::titleFont;

    // set UI colors
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.23f, 0.26f, 0.94f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.21f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.58f, 0.62f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.38f, 0.42f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.82f, 0.84f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.82f, 0.84f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.38f, 0.42f, 0.86f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.71f, 0.73f, 1.00f);

    // load ui elements
    fileTexture = loadTexture("assets/fileico.png", "assets/fileico.png");
    folderTexture = loadTexture("assets/folderico.png", "assets/folderico.png");

    // don't unload ui elements
    setTexturesStaticThreshold();

    // create viewport framebuffer
    glGenFramebuffers(1, &viewportFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportFramebuffer);

    glGenTextures(1, &viewportTex);
    glBindTexture(GL_TEXTURE_2D, viewportTex);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewportTex, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &viewportDepthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, viewportDepthBuf);
    glFramebufferRenderbuffer(
        GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, viewportDepthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    baseWindow = window;
    renderManager = RenderManager::getInstance();
}
GUIManager::~GUIManager() {
    if (projectThread.joinable())
        projectThread.join();

    glDeleteFramebuffers(1, &viewportFramebuffer);
    glDeleteTextures(1, &viewportTex);
}

// --- Custom Input Handlers ---
static void handleKeyboardInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Move the camera forward
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Move the camera backward
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // Strafe the camera left
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // Strafe the camera right
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 3);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // Ascend camera
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 4);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        // Descend camera
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 5);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        // Reset camera position
        renderManager->camera->resetPosition();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        // increase camera movement speed
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 6);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        // increase camera movement speed
        renderManager->camera->updateKeyboardInput(renderManager->deltaTime, 7);
    }
}

static void handleMouseInput(GLFWwindow* window) {
    glfwGetCursorPos(window, &renderManager->xPos, &renderManager->yPos);

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        if (renderManager->camera->focusState == false) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderManager->camera->focusState = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderManager->camera->focusState = false;
        }

        // if (renderManager->firstRClick  == true) {
        renderManager->xPosLast = renderManager->xPos;
        renderManager->yPosLast = renderManager->yPos;
        // renderManager->firstRClick = false;
        // }
    }

    if (renderManager->camera->focusState == true) {
        // now we can change the orientation of the camera

        // offset
        renderManager->xOffset = renderManager->xPos - renderManager->xPosLast;
        renderManager->yOffset = renderManager->yPos - renderManager->yPosLast;

        // send data to camera
        renderManager->camera->updateInput(
            renderManager->deltaTime, -1, renderManager->xOffset, renderManager->yOffset);

        renderManager->xPosLast = renderManager->xPos;
        renderManager->yPosLast = renderManager->yPos;
        glfwSetCursorPos(window, renderManager->xPosLast, renderManager->yPosLast);
    }
}

// --- GUI Widgets ---

// inline void drawAudioDemo() {
//     if (ImGui::Begin("Audio Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
//         ImGui::PushFont(guicfg::regularFont);
//         ImGui::Text("Active Clip: %s", audioPath.c_str());
//         if (ImGui::Button("Load Audio File")) {
//             const char* filters[] = {"*.mp3", "*.ogg", "*.flac", "*.wav"};
//             const char* filterDesc = "Audio Files";
//             std::string path = fdutil::openFile(
//                 "Select File", NULL, sizeof(filters) / sizeof(filters[0]), filters, filterDesc);
//             if (!path.empty()) {
//                 audio::audioStopAll();
//                 // TODO replace with nicer uuid
//                 audioClip = audio::audioLoad(path.c_str(), path);
//                 if (audioClip >= 0) {
//                     audioPath = path;
//                     logging::logInfo("Opened audio file {}\n", path);
//                 }
//                 else {
//                     logging::logErr("Failed to load audio file ", path);
//                 }
//             }
//         }
//
//         // adjust source position, listening position is center
//         if (ImGui::SliderFloat3("Src Position", &audioPos.x, -1.f, 1.f, "%.3f", 1)) {
//             audio::audioSetPosition(audioPos);
//         }
//
//         // play audio with 3d effects
//         if (ImGui::Button("Play Audio File")) {
//             if (!audioPath.empty())
//                 audio::audioPlay(audioClip);
//         }
//         ImGui::PopFont();
//     }
//     ImGui::End();
// }

inline float drawMainMenu(const char* executablePath) {
    float barHeight = .0f;
    if (ImGui::BeginMainMenuBar()) {
        // ImGui::Text("Active Directory: %s", activePath.c_str());

        // file menu
        if (ImGui::BeginMenu("File##mainmenu")) {
            ImGui::PushFont(guicfg::regularFont);
            if (ImGui::MenuItem("Create Project")) {
                std::string path = fdutil::selectFolder("Create a Project", NULL);
                // wait for current op to finish
                if (!path.empty()) {
                    if (projectThread.joinable())
                        projectThread.join();
                    projectThread = std::thread(createProj, path);
                    activePath = path;
                    assetfolder::setActiveDirectory(path);
                    currAssetFolder = assetfolder::getAssetsRootDir();
                    queryAssetsFolder = true;
                    queryLevelsFolder = true;
                }
            }
            if (ImGui::MenuItem("Open Project")) {
                const char* filter = "project.json";
                std::string path = fdutil::openFile(
                    "Open Project", nullptr, 1, &filter, "Project File (project.json)");
                if (!path.empty()) {
                    if (projectThread.joinable())
                        projectThread.join();

                    // try to load project
                    std::string level = loadProjectFile(path.c_str());

                    // try to open level
                    if (!level.empty()) {
                        // get folder path and set as active
                        activePath = getFolder(path);
                        assetfolder::setActiveDirectory(activePath);
                        currAssetFolder = assetfolder::getAssetsRootDir();

                        // reset queries
                        queryAssetsFolder = true;
                        queryLevelsFolder = true;

                        loadLevel((activePath + "/" + level).c_str(), scene);

                        // set appropriate window title
                        glfwSetWindowTitle(baseWindow,
                            (assetfolder::getName(level.c_str()) + " - ONO Engine").c_str());
                    }
                }
            }
            if (ImGui::MenuItem("Save Project")) {
                if (!activePath.empty()) {
                    saveProjectFile((activePath + "/project.json").c_str());
                }
            }
            if (ImGui::MenuItem("Save Level")) {
                saveCurrentLevel();
            }
            ImGui::PopFont();
            ImGui::EndMenu();
        }

        // build menu
        if (ImGui::BeginMenu("Build##mainmenu")) {
            ImGui::PushFont(guicfg::regularFont);
            if (ImGui::MenuItem("Build and Run")) {
                if (!activePath.empty()) {
                    // wait for current op to finish
                    if (projectThread.joinable())
                        projectThread.join();

                    projectThread = std::thread(buildRunProj, activePath, executablePath);
                }
            }
            ImGui::PopFont();
            ImGui::EndMenu();
        }

        // get height
        barHeight = ImGui::GetWindowSize().y;

        ImGui::EndMainMenuBar();
    }

    return barHeight;
}

inline void drawModelDemo() {
    if (ImGui::Begin("Model Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(guicfg::regularFont);
        ImGui::Text("Model Loading Demo");
        if (ImGui::Button("Load Model")) {
            std::string path = fdutil::openFile("Select Model File to Import", NULL, 0, NULL, NULL);
            // wait for current op to finish
            if (!path.empty()) {
                model.loadModel(path);
            }
        }
        ImGui::Text("Model Meshes: %llu", model.meshes.size());
        ImGui::Text("Model Textures: %llu", model.textures_loaded.size());
        ImGui::Text("From: %s", model.directory.c_str());
        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawLog() {
    if (ImGui::Begin("Log", NULL, NULL)) {
        ImGui::PushFont(guicfg::regularFont);
        ImGui::PushTextWrapPos(ImGui::GetWindowSize().x - 40.0f);
        ImGui::TextUnformatted(logging::getLogString());
        ImGui::PopTextWrapPos();
        if (logging::scrollToBot) {
            logging::scrollToBot = false;
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawTextureDebug() {
    // texture debug
    if (ImGui::Begin("Texture Debug", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(guicfg::regularFont);
        if (ImGui::Button("Load Texture")) {
            const char* filters[] = {"*.png", "*.jpg", "*.bmp", "*.tga", "*.hdr"};
            std::string path = fdutil::openFile(
                "Load Texture", NULL, sizeof(filters) / sizeof(filters[0]), filters, NULL);

            if (!path.empty()) {
                clearDynamicTextures();
                activeTexture = loadTexture(path.c_str(), path.c_str());
            }
        }
        if (activeTexture != -1) {
            const TextureInfo& texInfo = getTexture(activeTexture);
            ImGui::Image((void*)texInfo.id, ImVec2(400, 400));
        }
        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawAssetBrowser() {
    // folder debug
    if (ImGui::Begin("Asset Browser", NULL)) {
        ImGui::PushFont(guicfg::regularFont);
        ImGui::Text("Current Folder: %s", currAssetFolder.path.c_str());

        if (currAssetFolder.type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
            static std::vector<bool> itemIsSelected;
            int itemsPerLine = int(ImGui::GetWindowSize().x /
                (guicfg::assetMgrItemSize.x + guicfg::assetMgrPadding.x));

            // refresh folder if needed
            if (queryAssetsFolder) {
                assetfolder::listDir(currAssetFolder, folderItems);
                queryAssetsFolder = false;
                itemIsSelected.resize(folderItems.size(), false);
            }

            // import buttons
            if (ImGui::Button("Import File")) {
                std::vector<std::string> paths;
                fdutil::openFileMulti("Import File", NULL, 0, NULL, NULL, paths);
                if (!paths.empty()) {
                    assetfolder::addAssets(paths, currAssetFolder);
                    queryAssetsFolder = true;
                }
            }

            ImGui::SameLine();
            if (ImGui::Button("Import Folder")) {
                std::string path = fdutil::selectFolder("Import Folder", NULL);
                if (!path.empty()) {
                    assetfolder::addAsset(path, currAssetFolder);
                    queryAssetsFolder = true;
                }
            }

            // delete button
            ImGui::SameLine();
            if (ImGui::Button("Delete") || glfwGetKey(baseWindow, GLFW_KEY_DELETE) == GLFW_PRESS) {
                for (unsigned int i = 0; i < itemIsSelected.size(); i++) {
                    if (itemIsSelected[i])
                        assetfolder::delAsset(folderItems[i]);
                }
                setVec(itemIsSelected, false);
                queryAssetsFolder = true;
            }

            // outer dir shorcut
            ImGui::SameLine();
            if (ImGui::Button("Root")) {
                currAssetFolder = assetfolder::getAssetsRootDir();
                setVec(itemIsSelected, false);
                queryAssetsFolder = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("..")) {
                currAssetFolder = assetfolder::outerDir(currAssetFolder);
                setVec(itemIsSelected, false);
                queryAssetsFolder = true;
            }

            // refresh button
            ImGui::SameLine();
            if (ImGui::Button("Refresh")) {
                queryAssetsFolder = true;
            }

            // draw files in folder
            ImVec2 winPos = ImGui::GetWindowPos();
            float initialItemPad = ImGui::GetCursorPosX();
            for (unsigned int i = 0; i < folderItems.size(); i++) {
                ImGui::PushID(i);
                ImVec2 initialPos = ImGui::GetCursorPos();
                ImGui::BeginGroup();
                // icon
                if (folderItems[i].type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
                    const TextureInfo& texInfo = getTexture(folderTexture);
                    ImGui::Image((void*)texInfo.id, guicfg::assetMgrIconSize);
                }
                else {
                    const TextureInfo& texInfo = getTexture(fileTexture);
                    ImGui::Image((void*)texInfo.id, guicfg::assetMgrIconSize);
                }

                // filename
                ImGui::PushClipRect(ImVec2(winPos.x + initialPos.x, winPos.y + initialPos.y),
                    ImVec2(winPos.x + initialPos.x + guicfg::assetMgrItemSize.x,
                        winPos.y + initialPos.y + guicfg::assetMgrItemSize.y),
                    false);
                ImGui::Text("%s", folderItems[i].name.c_str());
                ImGui::PopClipRect();

                // selectable
                ImGui::SetCursorPos(initialPos);
                if (ImGui::Selectable(
                        "##fileselector", itemIsSelected[i], 0, guicfg::assetMgrItemSize)) {
                    if (glfwGetKey(baseWindow, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
                        glfwGetKey(baseWindow, GLFW_KEY_RIGHT_CONTROL)) {
                        itemIsSelected[i] = true;
                    }
                    else {
                        setVec(itemIsSelected, false);
                        itemIsSelected[i] = true;
                    }
                }

                ImGui::EndGroup();

                if (ImGui::IsItemHovered()) {
                    // check for folder switch
                    if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) &&
                        folderItems[i].type == assetfolder::AssetDescriptor::EFileType::FOLDER) {
                        currAssetFolder = folderItems[i];
                        setVec(itemIsSelected, false);
                        queryAssetsFolder = true;
                    }

                    // draw tooltip - full path
                    ImGui::SetTooltip("%s", folderItems[i].path.c_str());
                }

                if (i < folderItems.size() - 1) {
                    // wrap to next line if needed
                    bool wrap = i % itemsPerLine == itemsPerLine - 1;
                    ImVec2 nextPos = ImVec2(wrap ? initialItemPad
                                                 : (initialPos.x + guicfg::assetMgrItemSize.x +
                                                       guicfg::assetMgrPadding.x),
                        initialPos.y +
                            (guicfg::assetMgrIconSize.y + guicfg::assetMgrPadding.y) * wrap);
                    ImGui::SetCursorPos(nextPos);
                }

                ImGui::PopID();
            }
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsAnyItemHovered()) {
                setVec(itemIsSelected, false);
            }
        }
        ImGui::PopFont();
    }
    ImGui::End();
}
int viewportTexWidth = 0;
int viewportTexHeight = 0;

inline void drawViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
    if (ImGui::Begin("Viewport")) {
        // send input to renderer if window is hovered
        if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused() ||
            renderManager->camera->focusState) {
            handleKeyboardInput(baseWindow);
            handleMouseInput(baseWindow);
        }

        // adjust for titlebar
        ImVec2 windowSize = ImGui::GetWindowSize();
        windowSize.y -= ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;

        viewportTexWidth = int(windowSize.x);
        viewportTexHeight = int(windowSize.y);

        // adjust to window resize
        glBindTexture(GL_TEXTURE_2D, viewportTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportTexWidth, viewportTexHeight, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glBindRenderbuffer(GL_RENDERBUFFER, viewportDepthBuf);
        glRenderbufferStorage(
            GL_RENDERBUFFER, GL_DEPTH_STENCIL, viewportTexWidth, viewportTexHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // draw viewport
        ImGui::Image((void*)viewportTex, windowSize, ImVec2(0, 1), ImVec2(1, 0));
    }
    ImGui::PopStyleVar(2);
    ImGui::End();
}

inline void drawStyleEditor() {
    if (ImGui::Begin("Style Editor")) {
        ImGui::PushFont(guicfg::regularFont);
        ImGui::ShowStyleEditor(&guiStyle);
        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawEntities() {
    if (ImGui::Begin("Entities")) {
        ImGui::PushFont(guicfg::regularFont);

        // drawing info vars
        int openDepth = 0;
        std::vector<int> depths(scene.entities.size());
        setVec(depths, 0);

        // context menu vars
        BaseEntity targetChild;
        int targetParent = -1;
        int targetToDelete = -1;

        static int entityPayload = -1;

        for (unsigned int i = 0; i < scene.entities.size(); i++) {
            // get current node depth
            int currDepth = scene.entities[i].parent < 0 ? 0 : depths[scene.entities[i].parent] + 1;
            depths[i] = currDepth;

            // check if node is leaf based on next element
            int isLeaf = (i == scene.entities.size() - 1);
            if (isLeaf == 0) {
                int nextDepth =
                    scene.entities[i + 1].parent < 0 ? 0 : depths[scene.entities[i + 1].parent] + 1;
                isLeaf = (nextDepth <= currDepth);
            }

            // close all previous nodes at same or higher depth
            while (currDepth < openDepth) {
                ImGui::TreePop();
                ImGui::PopID();
                openDepth--;
            }

            if (currDepth <= openDepth) {
                // start new tree node if visible
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(scene.entities.at(i).name.c_str(),
                        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_OpenOnDoubleClick |
                            (ImGuiTreeNodeFlags_Leaf * isLeaf),
                        "%s", scene.entities[i].name.c_str())) {
                    openDepth = currDepth + 1;
                }
                else {
                    ImGui::PopID();
                }

                // handle select
                if (ImGui::IsItemClicked(ImGuiMouseButton_Left)) {
                    scene.selectedEntity = &scene.entities[i];
                }

                // handle drag and drop
                if (ImGui::BeginDragDropSource()) {
                    entityPayload = i;
                    ImGui::SetDragDropPayload("entityIdx", &entityPayload, sizeof(int));
                    ImGui::Text("%s", scene.entities[i].name.c_str());
                    ImGui::EndDragDropSource();
                }

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* target = ImGui::AcceptDragDropPayload("entityIdx"))
                        scene.setParent(*((int*)target->Data), i);
                    ImGui::EndDragDropTarget();
                }

                // handle context menu
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::MenuItem("Add Child Entity")) {
                        targetChild = BaseEntity();
                        targetParent = i;
                    }
                    if (ImGui::MenuItem("Add Child Camera Entity")) {
                        targetChild = CameraEntity();
                        targetParent = i;
                    }
                    if (ImGui::MenuItem("Add Child Model Entity")) {
                        targetChild = ModelEntity();
                        targetParent = i;
                    }
                    if (ImGui::MenuItem("Add Child Skeletal Mesh Entity")) {
                        targetChild = SkeletalMeshEntity();
                        targetParent = i;
                    }
                    if (ImGui::MenuItem("Delete Entity")) {
                        targetToDelete = i;
                    }
                    ImGui::EndPopup();
                }
            }
        }

        // close off all remaining tree nodes
        while (openDepth > 0) {
            ImGui::TreePop();
            ImGui::PopID();
            openDepth--;
        }

        // add new child if requested
        if (targetParent >= 0) {
            scene.selectedEntity = nullptr;
            scene.addChild(targetChild, targetParent);
        }

        // delete entity if requested
        if (targetToDelete >= 0) {
            scene.removeEntityByIdx(targetToDelete);
            scene.selectedEntity = nullptr;
        }

        // draw invisible button to allow context menu for full window
        // TODO - adjust size and pos when entity children are implemented
        ImVec2 restPos = ImGui::GetCursorPos();
        ImVec2 buttonSize = ImGui::GetWindowSize();
        float borderSize = ImGui::GetStyle().WindowBorderSize;
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        buttonSize.x -= borderSize + 2 * padding.x + restPos.x;
        buttonSize.y -= borderSize + 2 * padding.y + restPos.y;

        ImGui::InvisibleButton("##entitiesinvisblebutton", buttonSize);

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* target = ImGui::AcceptDragDropPayload("entityIdx"))
                scene.setParent(*((int*)target->Data), -1);
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Add Entity")) {
                scene.addEntity(BaseEntity());
                scene.selectedEntity = nullptr;
            }
            if (ImGui::MenuItem("Add Camera Entity")) {
                scene.addEntity(CameraEntity());
                scene.selectedEntity = nullptr;
            }
            if (ImGui::MenuItem("Add Model Entity")) {
                scene.addEntity(ModelEntity());
                scene.selectedEntity = nullptr;
            }
            if (ImGui::MenuItem("Add Skeletal Mesh Entity")) {
                scene.addEntity(SkeletalMeshEntity());
                scene.selectedEntity = nullptr;
            }
            ImGui::EndPopup();
        }
        ImGui::PopFont();
    }

    ImGui::End();
}

void drawComponentProps(TransformComponent& component) {
    ImGui::InputFloat3("Position", &component.position[0]);
    ImGui::InputFloat4("Rotation", &component.rotation[0]);
    ImGui::InputFloat3("Scale", &component.scale[0]);
}

void drawComponentProps(AudioSourceComponent& component) {
    // clip selector
    std::string previewPath = "";
    std::string previewStr = "None";
    if (component.clipDescriptor && component.clipDescriptor->path) {
        previewPath = *component.clipDescriptor->path;
        previewStr = assetfolder::getRelativePath(previewPath.c_str());
    }

    if (ImGui::BeginCombo("Audio File", previewStr.c_str())) {
        // get available audio clips
        static std::vector<assetfolder::AssetDescriptor> audioFiles;
        assetfolder::findAssetsByType(assetfolder::AssetDescriptor::EFileType::AUDIO, audioFiles);

        // list available audio clips
        for (unsigned int i = 0; i < audioFiles.size(); i++) {
            ImGui::PushID(i);
            bool isSelected = (previewPath == audioFiles[i].path);
            if (ImGui::Selectable(audioFiles[i].name.c_str(), &isSelected)) {
                // check if we need to load file
                // TODO better unique id scheme
                std::string uuid = assetfolder::getRelativePath(audioFiles[i].path.c_str());
                auto desc = audio::audioGetByUuid(uuid);

                if (!desc) {
                    // load file from disk
                    desc = audio::audioLoad(audioFiles[i].path.c_str(), uuid);
                }

                std::swap(component.clipDescriptor, desc);
                component.clipUuid = component.clipDescriptor ? uuid : "";
            }

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    // sound demo
    ImGui::BeginDisabled(!component.clipDescriptor);
    if (ImGui::Button("Start")) {
        audio::audioStopAll();
        component.startSound();
    }

    ImGui::SameLine();
    if (ImGui::Button("Stop")) {
        audio::audioStopAll();
    }
    ImGui::EndDisabled();

    // sound options
    ImGui::Checkbox("Play on Start", &component.playOnStart);
    ImGui::Checkbox("Loop", &component.loop);
    ImGui::Checkbox("Directional", &component.directional);
}

inline void drawProperties() {
    if (ImGui::Begin("Properties")) {
        ImGui::PushFont(guicfg::regularFont);
        // TODO properties for other item types

        if (scene.selectedEntity == nullptr) {
            ImGui::Text("Select an Entity to show it's components");
        }
        else {
            ImGui::InputText("Name", &scene.selectedEntity->name);
            // entitity transform
            ImGui::InputFloat3("Position", &scene.selectedEntity->position[0]);
            ImGui::InputFloat4("Rotation", &scene.selectedEntity->rotation[0]);
            ImGui::InputFloat3("Scale", &scene.selectedEntity->scale[0]);

            ImGui::Separator();

            // component lists

            // AudioSourceComponent
            std::vector<AudioSourceComponent>& audioSrcComponents =
                scene.selectedEntity->components.vecAudioSourceComponent;
            int componentToDelete = -1;
            for (unsigned int i = 0; i < audioSrcComponents.size(); i++) {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(audioSrcComponents[i].name.c_str(),
                        ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                            ImGuiTreeNodeFlags_DefaultOpen)) {

                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Add Audio Source Component")) {
                            scene.selectedEntity->components.addComponent(AudioSourceComponent());
                        }
                        if (ImGui::MenuItem("Add Script Component")) {
                        }
                        if (ImGui::MenuItem("Add Transform Component")) {
                            scene.selectedEntity->components.addComponent(TransformComponent());
                        }
                        if (ImGui::MenuItem("Delete Component")) {
                            componentToDelete = i;
                        }
                        ImGui::EndPopup();
                    }

                    drawComponentProps(audioSrcComponents[i]);
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            // handle delete requests
            if (componentToDelete >= 0) {
                audioSrcComponents.erase(audioSrcComponents.begin() + componentToDelete);
            }

            // TransformComponent
            std::vector<TransformComponent>& transformComponents =
                scene.selectedEntity->components.vecTransformComponent;
            componentToDelete = -1;
            for (unsigned int i = 0; i < transformComponents.size(); i++) {
                ImGui::PushID(i);
                if (ImGui::TreeNodeEx(transformComponents[i].name.c_str(),
                        ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow |
                            ImGuiTreeNodeFlags_OpenOnDoubleClick)) {

                    if (ImGui::BeginPopupContextItem()) {
                        if (ImGui::MenuItem("Add Audio Source Component")) {
                            scene.selectedEntity->components.addComponent(AudioSourceComponent());
                        }
                        if (ImGui::MenuItem("Add Script Component")) {
                        }
                        if (ImGui::MenuItem("Add Transform Component")) {
                            scene.selectedEntity->components.addComponent(TransformComponent());
                        }
                        if (ImGui::MenuItem("Delete Component")) {
                            componentToDelete = i;
                        }
                        ImGui::EndPopup();
                    }
                    drawComponentProps(transformComponents[i]);

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            // handle delete requests
            if (componentToDelete >= 0) {
                transformComponents.erase(transformComponents.begin() + componentToDelete);
            }

            // Other Component Types...

            // Context Menu
            ImVec2 currPos = ImGui::GetCursorPos();
            ImVec2 buttonSize = ImGui::GetWindowSize();
            float borderSize = ImGui::GetStyle().WindowBorderSize;
            ImVec2 padding = ImGui::GetStyle().WindowPadding;
            buttonSize.x -= borderSize + 2 * padding.x + currPos.x;
            buttonSize.y -= borderSize + 2 * padding.y + currPos.y;

            ImGui::InvisibleButton("##propertiesinvisblebutton", buttonSize);
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Add Audio Source Component")) {
                    scene.selectedEntity->components.addComponent(AudioSourceComponent());
                }
                if (ImGui::MenuItem("Add Script Component")) {
                }
                if (ImGui::MenuItem("Add Transform Component")) {
                    scene.selectedEntity->components.addComponent(TransformComponent());
                }
                ImGui::EndPopup();
            }
        }

        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawLevels() {
    static assetfolder::AssetDescriptor currLevelDir = {
        "", "", assetfolder::AssetDescriptor::EFileType::INVALID};
    static std::vector<assetfolder::AssetDescriptor> levelDescriptors;
    static int selectedLevel = -1;

    if (ImGui::Begin("Levels")) {
        ImGui::PushFont(guicfg::regularFont);
        // refresh levels if needed
        if (queryLevelsFolder) {
            currLevelDir = assetfolder::getLevelsRootDir();
            if (currLevelDir.type == assetfolder::AssetDescriptor::EFileType::FOLDER)
                assetfolder::listDir(currLevelDir, levelDescriptors);
            else
                levelDescriptors.clear();

            queryLevelsFolder = false;
            selectedLevel = -1;
        }

        // prepare ui sizes
        ImVec2 buttonSize = ImGui::GetWindowSize();
        float borderSize = ImGui::GetStyle().WindowBorderSize;
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        buttonSize.x -= borderSize + 2 * padding.x;

        // mv==rename in **NIX
        static int mvIdx;
        static std::string mvNameBuf;
        static bool mvIsCurr = false;
        static bool mvIsDef = false;

        // rename popup
        if (ImGui::BeginPopup("##mvlvlpopup")) {
            if (ImGui::InputText("Name", &mvNameBuf,
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll)) {
                std::string newPath = currLevelDir.path + "/" + mvNameBuf;
                std::rename(levelDescriptors[mvIdx].path.c_str(), newPath.c_str());

                if (mvIsDef) {
                    // update default level
                    setDefaultLevel(newPath);
                    saveProjectFile((activePath + "/project.json").c_str());
                }

                if (mvIsCurr) {
                    // update active level
                    loadLevel(newPath.c_str(), scene);
                    glfwSetWindowTitle(baseWindow, (mvNameBuf + " - ONO Engine").c_str());
                }

                queryLevelsFolder = true;
                ImGui::CloseCurrentPopup();
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        bool useMvPopup = false;
        // draw levels
        for (unsigned int i = 0; i < levelDescriptors.size(); i++) {
            ImGui::PushID(i);
            ImVec2 initialPos = ImGui::GetCursorPos();
            BeginGroup();
            // text
            ImGui::Text("%s", levelDescriptors[i].name.c_str());

            bool isCurrLvl = isCurrentLevel(levelDescriptors[i].name);
            bool isDefLvl = isDefaultLevel(levelDescriptors[i].name);

            if (isCurrLvl) {
                ImGui::SameLine();
                ImGui::Text("Current");
            }

            if (isDefLvl) {
                ImGui::SameLine();
                ImGui::Text("Default");
            }

            ImGui::SetCursorPos(initialPos);

            // selectable over text
            bool selected = (i == selectedLevel);
            if (ImGui::Selectable("##levelselectable", &selected, 0,
                    ImVec2(buttonSize.x, ImGui::GetItemRectSize().y))) {
                selectedLevel = i;
            }

            // popup
            if (ImGui::BeginPopupContextItem()) {
                if (ImGui::MenuItem("Load Level")) {
                    // load level and change window title
                    loadLevel(levelDescriptors[i].path.c_str(), scene);

                    glfwSetWindowTitle(
                        baseWindow, (levelDescriptors[i].name + " - ONO Engine").c_str());
                }

                if (ImGui::MenuItem("Rename")) {
                    mvIdx = i;
                    mvIsDef = isDefLvl;
                    mvIsCurr = isCurrLvl;
                    mvNameBuf = levelDescriptors[i].name;
                    useMvPopup = true;
                }

                if (ImGui::MenuItem("Delete")) {
                    std::remove(levelDescriptors[i].path.c_str());
                    if (levelDescriptors.size() == 1) {
                        // last descriptor - generate new empty one
                        saveLevel((currLevelDir.path + "/default.json").c_str(), Scene());
                        setDefaultLevel(currLevelDir.path + "/default.json");
                        saveProjectFile((activePath + "/project.json").c_str());
                        loadLevel((currLevelDir.path + "/default.json").c_str(), scene);
                    }
                    else if (isCurrLvl) {
                        // current level deleted - switch to another
                        int nextIdx = (i == 0) ? i + 1 : i - 1;
                        loadLevel(levelDescriptors[nextIdx].path.c_str(), scene);
                    }
                    else if (isDefLvl) {
                        // default level deleted - switch default
                        int nextIdx = (i == 0) ? i + 1 : i - 1;
                        setDefaultLevel(levelDescriptors[nextIdx].path.c_str());
                        saveProjectFile((activePath + "/project.json").c_str());
                    }
                    queryLevelsFolder = true;
                }

                if (ImGui::MenuItem("Set as Default")) {
                    setDefaultLevel(levelDescriptors[i].path);
                    saveProjectFile((activePath + "/project.json").c_str());
                }
                ImGui::EndPopup();
            }
            EndGroup();
            ImGui::PopID();
        }

        // check for rename popup
        if (useMvPopup)
            ImGui::OpenPopup("##mvlvlpopup");

        // define new level popup
        if (ImGui::BeginPopup("##newlvlpopup")) {
            std::string buf;
            if (ImGui::InputText("Name ##newlvl", &buf,
                    ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll)) {
                // save new empty level
                std::string lvlName = currLevelDir.path + "/" + buf + ".json";
                saveLevel(lvlName.c_str(), Scene());
                queryLevelsFolder = true;
                ImGui::CloseCurrentPopup();
            }
            else if (ImGui::IsKeyPressed(ImGuiKey_Escape))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        // check for new level popup
        if (ImGui::Button("New Level"))
            ImGui::OpenPopup("##newlvlpopup");

        // //draw invisible button for clicking rest of empty widget space
        // buttonSize.y -= borderSize + padding.y + ImGui::GetCursorPosY();
        // if (ImGui::InvisibleButton("##entitiesinvisblebutton", buttonSize)) {
        //     selectedLevel = -1;
        // }
        // if (ImGui::BeginPopupContextItem()) {
        //     if (ImGui::MenuItem("New Level")) {}
        //     ImGui::EndPopup();
        // }

        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawScriptDemo() {
    if (ImGui::Begin("Scripting")) {
        ImGui::PushFont(guicfg::regularFont);
        if (ImGui::Button("Run Script from File")) {
            std::string path = fdutil::openFile("Open Script", nullptr, 0, nullptr, nullptr);
            if (!path.empty()) {
                scripting::runScript(path.c_str());
            }
        }
        ImGui::PopFont();
    }
    ImGui::End();
}

void drawStats() {
    if (ImGui::Begin("Statistics")) {
        ImGui::PushFont(guicfg::regularFont);
        ImGui::Text("AUDIO: %i clips loaded", audio::getAudioClipCount());
        ImGui::PopFont();
    }
    ImGui::End();
}

void prepUI(GLFWwindow* window, const char* executablePath, float dt, int viewportWidth,
    int viewportHeight) {
    ImVec2 windowSize = ImVec2(float(viewportWidth), float(viewportHeight));
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // ImVec2 mousePos = ImGui::GetMousePos();

    // adjust fullscreen windows size to account for menubar
    float mainMenuHeight = drawMainMenu(executablePath);
    windowSize.y -= mainMenuHeight;

    static ImGuiID dockCenter = 0;
    static ImGuiID dockLeft;
    static ImGuiID dockRight;
    static ImGuiID dockBotRight;
    static ImGuiID dockBotLeft;

    // TODO save and load dock state
    // create layout if not present already
    ImGui::SetNextWindowPos(ImVec2(.0f, mainMenuHeight));
    ImGui::SetNextWindowSize(windowSize);
    if (ImGui::Begin("##FullscreenWindow", nullptr,
            ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoDocking)) {

        ImGuiID dockId = ImGui::GetID("DockspaceDefault");
        ImGui::DockSpace(dockId);
        static bool dockSpaceInit = false;
        if (!dockSpaceInit) {
            // create initial empty node
            ImGui::DockBuilderRemoveNode(dockId);
            dockCenter = ImGui::DockBuilderAddNode(dockId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(dockCenter, windowSize);
            ImGui::DockBuilderSetNodePos(dockCenter, ImGui::GetMainViewport()->Pos);

            // split vertically
            dockBotRight =
                ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, .4f, NULL, &dockCenter);
            dockBotLeft =
                ImGui::DockBuilderSplitNode(dockBotRight, ImGuiDir_Left, .5f, NULL, &dockBotRight);

            // split horizontally twice
            dockLeft =
                ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left, .25f, NULL, &dockCenter);
            dockRight =
                ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, .25f, NULL, &dockCenter);

            ImGui::DockBuilderFinish(dockId);
            dockSpaceInit = true;
        }
    }

    ImGui::End();

    // TODO undocked windows in front
    ImGui::SetNextWindowDockID(dockLeft, ImGuiCond_Once);
    drawLevels();

    ImGui::SetNextWindowDockID(dockLeft, ImGuiCond_Once);
    drawEntities();

    ImGui::SetNextWindowDockID(dockBotLeft, ImGuiCond_Once);
    drawAssetBrowser();

    ImGui::SetNextWindowDockID(dockBotRight, ImGuiCond_Once);
    drawStats();

    ImGui::SetNextWindowDockID(dockBotRight, ImGuiCond_Once);
    drawLog();

    ImGui::SetNextWindowDockID(dockCenter, ImGuiCond_Once);
    drawViewport();

    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Once);
    drawScriptDemo();

    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Once);
    drawStyleEditor();

    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Once);
    drawProperties();

    // prepare gui for rendering
    ImGui::Render();
}

void drawUI() {
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}
