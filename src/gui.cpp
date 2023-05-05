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
#include <implot.h>
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
#include "metrics.h"
#include "util.h"
#include "scripting.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "asset_import/animation.h"
#include "model_import/model.h"
#include "ECS/Component/AudioSourceComponent.h"
#include "ECS/Component/CameraComponent.h"
#include "ECS/Component/ModelComponent.h"
#include "ECS/Component/SkeletalModelComponent.h"
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

// --- Internal GUI State ---

// base functionality vars
static GLFWwindow* baseWindow;
static std::string activePath;
static std::thread projectThread;

// texture demo vars
static std::shared_ptr<TextureDescriptor> activeTexture;

// audio demo vars
static int audioClip = -1;
static std::string audioPath("");
static glm::vec3 audioPos(.0f);

// asset manager controls
static assetfolder::AssetDescriptor currAssetFolder = {"", "",
    assetfolder::AssetDescriptor::EFileType::INVALID};
static std::vector<assetfolder::AssetDescriptor> folderItems;
static bool queryAssetsFolder = true;
static bool queryLevelsFolder = true;

// ui textures
static std::shared_ptr<TextureDescriptor> fileTexture;
static std::shared_ptr<TextureDescriptor> folderTexture;

// model demo vars
// static Model model;

// renderer vars
static RenderManager* renderManager;

// editor vars
Scene scene;

// viewport widget vars
GLuint viewportMultisampleFramebuffer;
GLuint viewportFramebuffer;
static GLuint viewportMultisampleTex;
static GLuint viewportMultisampleDepthBuf;
static GLuint viewportResolveTex;

// TODO Load/Save style to disk
static ImGuiStyle guiStyle;

// --- Module Init/Deinit
GUIManager::GUIManager(GLFWwindow* window) {
    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
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

    // create viewport framebuffers

    // multisample
    glGenFramebuffers(1, &viewportMultisampleFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportMultisampleFramebuffer);

    // glGenTextures(1, &viewportMultisampleTex);
    // glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, viewportMultisampleTex);
    // glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, 200, 200, GL_TRUE);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE,
    //     viewportMultisampleTex, 0);
    glGenRenderbuffers(1, &viewportMultisampleTex);
    glBindRenderbuffer(GL_RENDERBUFFER, viewportMultisampleTex);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_RGBA, 200, 200);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
        viewportMultisampleTex);

    glGenRenderbuffers(1, &viewportMultisampleDepthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, viewportMultisampleDepthBuf);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_STENCIL, 200, 200);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
        viewportMultisampleDepthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // resolve
    glGenFramebuffers(1, &viewportFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, viewportFramebuffer);
    glGenTextures(1, &viewportResolveTex);
    glBindTexture(GL_TEXTURE_2D, viewportResolveTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 200, 200, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, viewportResolveTex,
        0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    baseWindow = window;
    renderManager = RenderManager::getInstance();
}
GUIManager::~GUIManager() {
    if (projectThread.joinable())
        projectThread.join();

    glDeleteFramebuffers(1, &viewportMultisampleFramebuffer);
    glDeleteTextures(1, &viewportMultisampleTex);
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

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
        activePath = path;
        assetfolder::setActiveDirectory(path);
        currAssetFolder = assetfolder::getAssetsRootDir();
        std::string level = loadProjectFile((path + "/project.json").c_str());
        if (!level.empty())
            loadLevel((activePath + "/" + level).c_str(), scene);

        queryAssetsFolder = true;
        queryLevelsFolder = true;
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

// --- Custom Input Handlers ---
static void handleKeyboardInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        // Move the camera forward
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        // Move the camera backward
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        // Strafe the camera left
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 2);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        // Strafe the camera right
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 3);
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        // Ascend camera
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 4);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        // Descend camera
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 5);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        // Reset camera position
        renderManager->camera.resetPosition();
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        // increase camera movement speed
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 6);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        // increase camera movement speed
        renderManager->camera.updateKeyboardInput(renderManager->deltaTime, 7);
    }
}

static void handleMouseInput(GLFWwindow* window) {
    glfwGetCursorPos(window, &renderManager->xPos, &renderManager->yPos);

    // if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
        if (renderManager->camera.focusState == false) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            renderManager->camera.focusState = true;
        }
        else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            renderManager->camera.focusState = false;
        }

        // if (renderManager->firstRClick  == true) {
        renderManager->xPosLast = renderManager->xPos;
        renderManager->yPosLast = renderManager->yPos;
        // renderManager->firstRClick = false;
        // }
    }

    if (renderManager->camera.focusState == true) {
        // now we can change the orientation of the camera

        // offset
        renderManager->xOffset = renderManager->xPos - renderManager->xPosLast;
        renderManager->yOffset = renderManager->yPos - renderManager->yPosLast;

        // send data to camera
        renderManager->camera.updateInput(renderManager->deltaTime, -1, renderManager->xOffset,
            renderManager->yOffset);

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
                }
            }
            if (ImGui::MenuItem("Open Project")) {
                const char* filter = "project.json";
                std::string path = fdutil::openFile("Open Project", nullptr, 1, &filter,
                    "Project File (project.json)");
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

// inline void drawModelDemo() {
//     if (ImGui::Begin("Model Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
//         ImGui::PushFont(guicfg::regularFont);
//         ImGui::Text("Model Loading Demo");
//         if (ImGui::Button("Load Model")) {
//             std::string path = fdutil::openFile("Select Model File to Import", NULL, 0, NULL,
//             NULL);
//             // wait for current op to finish
//             if (!path.empty()) {
//                 model.loadModel(path);
//             }
//         }
//         ImGui::Text("Model Meshes: %llu", model.meshes.size());
//         ImGui::Text("Model Textures: %llu", model.textures_loaded.size());
//         ImGui::Text("From: %s", model.directory.c_str());
//         ImGui::PopFont();
//     }
//     ImGui::End();
// }

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
            std::string path = fdutil::openFile("Load Texture", NULL,
                sizeof(filters) / sizeof(filters[0]), filters, NULL);

            if (!path.empty()) {
                activeTexture = loadTexture(path.c_str(), path.c_str());
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear")) {
            activeTexture.reset();
        }
        if (activeTexture) {
            ImGui::Image((void*)activeTexture->texId, ImVec2(400, 400));
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
                    ImGui::Image((void*)folderTexture->texId, guicfg::assetMgrIconSize);
                }
                else {
                    ImGui::Image((void*)fileTexture->texId, guicfg::assetMgrIconSize);
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
                if (ImGui::Selectable("##fileselector", itemIsSelected[i], 0,
                        guicfg::assetMgrItemSize)) {
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

static int viewportSamples = 4;

inline void drawViewport() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(.0f, .0f));
    if (ImGui::Begin("Viewport")) {
        // send input to renderer if window is hovered
        if (ImGui::IsWindowHovered() || ImGui::IsWindowFocused() ||
            renderManager->camera.focusState) {
            handleKeyboardInput(baseWindow);
            handleMouseInput(baseWindow);
        }

        // adjust for titlebar
        ImVec2 windowSize = ImGui::GetWindowSize();
        windowSize.y -= ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2;

        viewportTexWidth = int(windowSize.x);
        viewportTexHeight = int(windowSize.y);

        // samples for antialiasing
        GLuint samples = 4;

        // adjust to window resize
        glBindRenderbuffer(GL_RENDERBUFFER, viewportMultisampleTex);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, viewportSamples, GL_RGBA,
            viewportTexWidth, viewportTexHeight);

        glBindRenderbuffer(GL_RENDERBUFFER, viewportMultisampleDepthBuf);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, viewportSamples, GL_DEPTH_STENCIL,
            viewportTexWidth, viewportTexHeight);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);

        glBindTexture(GL_TEXTURE_2D, viewportResolveTex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, viewportTexWidth, viewportTexHeight, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        ImVec2 initialPos = ImGui::GetCursorPos();

        // draw viewport
        ImGui::Image((void*)viewportResolveTex, windowSize, ImVec2(0, 1), ImVec2(1, 0));

        // draw sample selector
        ImGui::SetCursorPos(initialPos);
        ImGui::PushFont(guicfg::regularFont);
        ImGui::SetNextItemWidth(50);
        if (ImGui::BeginCombo("Samples", std::to_string(viewportSamples).c_str())) {
            for (int i = 1; i < GL_MAX_SAMPLES && i <= 32; i *= 2) {
                if (ImGui::Selectable(std::to_string(i).c_str(), i == viewportSamples))
                    viewportSamples = i;
            }
            ImGui::EndCombo();
        }
        ImGui::PopFont();
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
                    scene.selectedCameraIdx = 0;
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
            scene.selectedCameraIdx = 0;
            scene.addChild(targetChild, targetParent);
        }

        // delete entity if requested
        if (targetToDelete >= 0) {
            scene.removeEntityByIdx(targetToDelete);
            scene.selectedEntity = nullptr;
            scene.selectedCameraIdx = 0;
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
                scene.selectedCameraIdx = 0;
            }
            if (ImGui::MenuItem("Add Camera Entity")) {
                scene.addEntity(CameraEntity());
                scene.selectedEntity = nullptr;
                scene.selectedCameraIdx = 0;
            }
            if (ImGui::MenuItem("Add Model Entity")) {
                scene.addEntity(ModelEntity());
                scene.selectedEntity = nullptr;
                scene.selectedCameraIdx = 0;
            }
            if (ImGui::MenuItem("Add Skeletal Mesh Entity")) {
                scene.addEntity(SkeletalMeshEntity());
                scene.selectedEntity = nullptr;
                scene.selectedCameraIdx = 0;
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

void drawComponentProps(ModelComponent& component) {
    std::string previewStr = "Select a Model";
    if (component.modelDescriptor && component.modelDescriptor->path) {
        previewStr = *component.modelDescriptor->path;
        previewStr = assetfolder::getRelativePath(previewStr.c_str());
    }

    if (ImGui::BeginCombo("Model File", previewStr.c_str())) {
        // get available audio clips
        static std::vector<assetfolder::AssetDescriptor> modelFiles;
        assetfolder::findAssetsByType(assetfolder::AssetDescriptor::EFileType::MODEL, modelFiles);

        // list available audio clips
        for (unsigned int i = 0; i < modelFiles.size(); i++) {
            ImGui::PushID(i);
            bool isSelected = (previewStr == modelFiles[i].path);
            if (ImGui::Selectable(modelFiles[i].name.c_str(), &isSelected)) {
                // check if we need to load file
                // TODO better unique id scheme
                std::string uuid = assetfolder::getRelativePath(modelFiles[i].path.c_str());
                auto desc = model::modelGetByUuid(uuid);

                if (!desc) {
                    // load file from disk
                    desc = model::modelLoad(modelFiles[i].path.c_str(), uuid);
                }

                std::swap(component.modelDescriptor, desc);
                component.modelUuid = component.modelDescriptor ? uuid : "";
            }
            ImGui::PopID();
        }

        ImGui::EndCombo();
    }
}

void drawComponentProps(SkeletalModelComponent& component) {
    bool hasModel = false;
    std::string previewStr = "Select a Model";
    if (component.modelDescriptor && component.modelDescriptor->path) {
        previewStr = *component.modelDescriptor->path;
        previewStr = assetfolder::getRelativePath(previewStr.c_str());
        hasModel = true;
    }

    if (ImGui::BeginCombo("Model File", previewStr.c_str())) {
        // get available audio clips
        static std::vector<assetfolder::AssetDescriptor> modelFiles;
        assetfolder::findAssetsByType(assetfolder::AssetDescriptor::EFileType::MODEL, modelFiles);

        // list available audio clips
        for (unsigned int i = 0; i < modelFiles.size(); i++) {
            ImGui::PushID(i);
            bool isSelected = (previewStr == modelFiles[i].path);
            if (ImGui::Selectable(modelFiles[i].name.c_str(), &isSelected)) {
                // check if we need to load file
                // TODO better unique id scheme
                std::string uuid = assetfolder::getRelativePath(modelFiles[i].path.c_str());
                auto desc = model::modelGetByUuid(uuid);

                if (!desc) {
                    // load file from disk
                    desc = model::modelLoad(modelFiles[i].path.c_str(), uuid);
                }

                std::swap(component.modelDescriptor, desc);
                component.modelUuid = component.modelDescriptor ? uuid : "";
            }
            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    if (!hasModel) {
        ImGui::Text("Select a Model with a Skeleton to view the animation controller properties");
        return;
    }

    ImGui::Columns(1);
    if (ImGui::Button("Add Node")) {
        component.addNode();
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
    for (unsigned int i = 0; i < component.nodes.size(); i++) {
        ImGui::PushID(i);
        ImGui::NextColumn();
        std::string previewStr2 = "Select an Animation";
        if (component.selectedNode && component.selectedNode->animationDescriptor &&
            component.selectedNode->animationDescriptor->path) {
            previewStr2 = *component.selectedNode->animationDescriptor->path;
            previewStr2 = assetfolder::getRelativePath(previewStr2.c_str());
        }

        if (ImGui::BeginCombo("##node_animation", previewStr2.c_str())) {
            // get available audio clips
            static std::vector<assetfolder::AssetDescriptor> animationFiles;
            assetfolder::findAssetsByType(assetfolder::AssetDescriptor::EFileType::MODEL,
                animationFiles);

            // list available audio clips
            for (unsigned int i = 0; i < animationFiles.size(); i++) {
                ImGui::PushID(i);
                bool isSelected = (previewStr2 == animationFiles[i].path);
                if (ImGui::Selectable(animationFiles[i].name.c_str(), &isSelected)) {
                    // check if we need to load file
                    // TODO better unique id scheme
                    std::string uuid = assetfolder::getRelativePath(animationFiles[i].path.c_str());
                    auto desc = animation::animationGetByUuid(uuid);

                    if (!desc) {
                        // load file from disk
                        desc = animation::animationLoad(animationFiles[i].path.c_str(), uuid,
                            component.modelDescriptor);
                    }

                    std::swap(component.selectedNode->animationDescriptor, desc);
                    component.selectedNode->animationUuid =
                        (component.selectedNode && component.selectedNode->animationDescriptor)
                        ? uuid
                        : "";
                }
                ImGui::PopID();
            }

            ImGui::EndCombo();
        }
        ImGui::NextColumn();
        ImGui::InputScalar("##loop_count", ImGuiDataType_U32, &component.nodes[i].loopCount);
        ImGui::NextColumn();
        if (ImGui::Button("Select Node")) {
            component.selectedNode = &component.nodes[i];
        }
        ImGui::NextColumn();
        if (ImGui::Button("Remove Node")) {
            component.removeNode(i);
        }
        ImGui::NextColumn();
        ImGui::PopID();
    }
    ImGui::Columns(1);
    if (ImGui::Button("Add No Condition Transition")) {
        component.addNoConditionTransition();
    }
    if (ImGui::Button("Add Boolean Transition")) {
        component.addBoolACTransition();
    }
    if (ImGui::Button("Add Integer Transition")) {
        component.addIntACTransition();
    }
    if (ImGui::Button("Add Float Transition")) {
        component.addFloatACTransition();
    }
    ImGui::Text("Transitions:");
    if (component.selectedNode) {
        ImGui::Columns(2);
        ImGui::Separator();
        ImGui::Text("Destination Node");
        ImGui::NextColumn();
        ImGui::Text("Remove Transition");
        ImGui::NextColumn();
        ImGui::Separator();
        for (unsigned int i = 0; i < component.selectedNode->noConditionTransitions.size(); i++) {
            ImGui::PushID(i);
            if (ImGui::BeginCombo("##transition_to_nocond",
                    component.selectedNode->noConditionTransitions[i].transitionTo->name.c_str())) {
                for (unsigned int j = 0; j < component.nodes.size(); j++) {
                    if (ImGui::Selectable(component.nodes[j].name.c_str(),
                            component.selectedNode->noConditionTransitions[i].transitionTo ==
                                &component.nodes[j])) {
                        component.selectedNode->noConditionTransitions[i].transitionTo =
                            &component.nodes[j];
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::NextColumn();
            if (ImGui::Button("Remove Transition")) {
                component.removeNoConditionTransition(i);
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
        for (unsigned int i = 0; i < component.selectedNode->boolTransitions.size(); i++) {
            ImGui::PushID(i);
            if (ImGui::BeginCombo("##transition_to_bool",
                    component.selectedNode->boolTransitions[i].transitionTo->name.c_str())) {
                for (unsigned int j = 0; j < component.nodes.size(); j++) {
                    if (ImGui::Selectable(component.nodes[j].name.c_str(),
                            component.selectedNode->boolTransitions[i].transitionTo ==
                                &component.nodes[j])) {
                        component.selectedNode->boolTransitions[i].transitionTo =
                            &component.nodes[j];
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::NextColumn();
            ImGui::Checkbox("##immediate_bool",
                &component.selectedNode->boolTransitions[i].immediate);
            ImGui::NextColumn();
            ImGui::Checkbox("##condition_bool",
                &component.selectedNode->boolTransitions[i].condition);
            ImGui::NextColumn();
            ImGui::Checkbox("##desired_bool",
                &component.selectedNode->boolTransitions[i].desiredValue);
            ImGui::NextColumn();
            if (ImGui::Button("Remove Transition")) {
                component.removeBoolACTransition(i);
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
        for (unsigned int i = 0; i < component.selectedNode->intTransitions.size(); i++) {
            ImGui::PushID(i);
            if (ImGui::BeginCombo("##transition_to_int",
                    component.selectedNode->intTransitions[i].transitionTo->name.c_str())) {
                for (unsigned int j = 0; j < component.nodes.size(); j++) {
                    if (ImGui::Selectable(component.nodes[j].name.c_str(),
                            component.selectedNode->intTransitions[i].transitionTo ==
                                &component.nodes[j])) {
                        component.selectedNode->intTransitions[i].transitionTo =
                            &component.nodes[j];
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::NextColumn();
            ImGui::Checkbox("##immediate_int",
                &component.selectedNode->intTransitions[i].immediate);
            ImGui::NextColumn();
            ImGui::Text("%d", component.selectedNode->intTransitions[i].condition);
            ImGui::NextColumn();
            ImGui::InputInt("##desired_int",
                &component.selectedNode->intTransitions[i].desiredValue);
            ImGui::NextColumn();
            ImGui::Checkbox("##should_lower_int",
                &component.selectedNode->intTransitions[i].shouldBeLower);
            ImGui::NextColumn();
            ImGui::Checkbox("##should_equal_int",
                &component.selectedNode->intTransitions[i].shouldBeEqual);
            ImGui::NextColumn();
            ImGui::Checkbox("##should_greater_int",
                &component.selectedNode->intTransitions[i].shouldBeGreater);
            ImGui::NextColumn();
            if (ImGui::Button("Remove Transition")) {
                component.removeIntACTransition(i);
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
        for (unsigned int i = 0; i < component.selectedNode->floatTransitions.size(); i++) {
            ImGui::PushID(i);
            if (ImGui::BeginCombo("##transition_to_float",
                    component.selectedNode->floatTransitions[i].transitionTo->name.c_str())) {
                for (unsigned int j = 0; j < component.nodes.size(); j++) {
                    if (ImGui::Selectable(component.nodes[j].name.c_str(),
                            component.selectedNode->floatTransitions[i].transitionTo ==
                                &component.nodes[j])) {
                        component.selectedNode->floatTransitions[i].transitionTo =
                            &component.nodes[j];
                    }
                }
                ImGui::EndCombo();
            }
            ImGui::NextColumn();
            ImGui::Checkbox("##immediate_float",
                &component.selectedNode->floatTransitions[i].immediate);
            ImGui::NextColumn();
            ImGui::Text("%f", component.selectedNode->floatTransitions[i].condition);
            ImGui::NextColumn();
            ImGui::InputFloat("##desired_float",
                &component.selectedNode->floatTransitions[i].desiredValue);
            ImGui::NextColumn();
            ImGui::Checkbox("##should_lower_float",
                &component.selectedNode->floatTransitions[i].shouldBeLower);
            ImGui::NextColumn();
            ImGui::Checkbox("##should_equal_float",
                &component.selectedNode->floatTransitions[i].shouldBeEqual);
            ImGui::NextColumn();
            ImGui::Checkbox("##should_greater_float",
                &component.selectedNode->floatTransitions[i].shouldBeGreater);
            ImGui::NextColumn();
            if (ImGui::Button("Remove Transition")) {
                component.removeFloatACTransition(i);
            }
            ImGui::NextColumn();
            ImGui::PopID();
        }
        ImGui::Columns(1);
    }
    else {
        ImGui::Text("Select a node to show it's transitions");
    }
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

static void drawEntitySelector(ScriptComponent& component, int i) {
    int idx = scene.uuidToIdx.count(component.args[i].arg._int) > 0
        ? scene.uuidToIdx[component.args[i].arg._int]
        : -1;
    std::string previewStr =
        (idx >= 0 && idx < scene.entities.size()) ? scene.entities[idx].name : "None";
    if (ImGui::BeginCombo("##entityselector", previewStr.c_str())) {
        for (unsigned int j = 0; j < scene.entities.size(); j++) {
            ImGui::PushID(j);
            bool isSelected = idx == j;
            if (ImGui::Selectable(scene.entities[j].name.c_str(), &isSelected)) {
                component.args[i].arg._int = scene.entities[j].uuid;
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}

template <typename T> void drawComponentSelector(int& idx, const std::vector<T>& components) {
    std::string previewStr = (idx >= 0 && idx < components.size()) ? components[idx].name : "None";
    if (ImGui::BeginCombo("##compselector", previewStr.c_str())) {
        for (unsigned int i = 0; i < components.size(); i++) {
            ImGui::PushID(i);
            bool isSelected = idx == i;
            if (ImGui::Selectable(components[i].name.c_str(), &isSelected)) {
                idx = i;
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}

static void drawComponentSelectorOuter(ScriptComponent& component, int i) {
    ComponentLocation& loc = component.args[i].arg._loc;
    int idx = scene.uuidToIdx.count(loc.entityIdx) > 0 ? scene.uuidToIdx[loc.entityIdx] : -1;
    // entity selector
    std::string previewStr =
        (idx >= 0 && idx < scene.entities.size()) ? scene.entities[idx].name : "None";
    if (ImGui::BeginCombo("##entityselector", previewStr.c_str())) {
        for (unsigned int j = 0; j < scene.entities.size(); j++) {
            ImGui::PushID(j);
            bool isSelected = loc.entityIdx == scene.entities[j].uuid;
            if (ImGui::Selectable(scene.entities[j].name.c_str(), &isSelected)) {
                loc.entityIdx = scene.entities[j].uuid;
                idx = scene.uuidToIdx[loc.entityIdx];
            }
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
    if (idx < 0 || idx >= scene.entities.size())
        return;

    // TODO rest of function is hard to maintain - try to generate this in the future
    // type selector
    const char* types[] = {"BaseComponent", "TransformComponent", "ScriptComponent",
        "CameraComponent", "AudioSourceComponent", "ModelComponent", "SkeletalModelComponent",
        "LightComponent"};
    previewStr =
        (loc.type >= 0 && loc.type < ComponentLocation::COMPTYPE_MAX) ? types[loc.type] : "None";
    if (ImGui::BeginCombo("##typeselector", previewStr.c_str())) {
        for (int j = 0; j < ComponentLocation::COMPTYPE_MAX; j++) {
            ImGui::PushID(j);
            bool isSelected = loc.type == j;
            if (ImGui::Selectable(types[j], &isSelected))
                loc.type = (ComponentLocation::CompType)j;
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }

    // component selector
    const ComponentStorage& storage = scene.entities[loc.entityIdx].components;
    switch (loc.type) {
    case ComponentLocation::BASECOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecBaseComponent);
        break;
    case ComponentLocation::TRANSFORMCOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecTransformComponent);
        break;
    case ComponentLocation::SCRIPTCOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecScriptComponent);
        break;
    case ComponentLocation::CAMERACOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecCameraComponent);
        break;
    case ComponentLocation::AUDIOSOURCECOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecAudioSourceComponent);
        break;
    case ComponentLocation::MODELCOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecModelComponent);
        break;
    case ComponentLocation::SKELETALMODELCOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecSkeletalModelComponent);
        break;
    case ComponentLocation::LIGHTCOMPONENT:
        drawComponentSelector(loc.componentIdx, storage.vecLightComponent);
        break;
    default:;
    }
}

void drawComponentProps(ScriptComponent& component) {
    std::string previewStr = component.scriptPath.empty()
        ? "None"
        : assetfolder::getRelativePath(component.scriptPath.c_str());

    // script selector
    if (ImGui::BeginCombo("Script File", previewStr.c_str())) {
        // get available audio clips
        static std::vector<assetfolder::AssetDescriptor> scriptFiles;
        assetfolder::findAssetsByType(assetfolder::AssetDescriptor::EFileType::SCRIPT, scriptFiles);

        // list available audio clips
        for (unsigned int i = 0; i < scriptFiles.size(); i++) {
            ImGui::PushID(i);
            bool isSelected = (component.scriptPath == scriptFiles[i].path);
            if (ImGui::Selectable(scriptFiles[i].name.c_str(), &isSelected)) {
                component.scriptPath = scriptFiles[i].path;
            }

            ImGui::PopID();
        }

        ImGui::EndCombo();
    }

    // argument list
    if (!component.scriptPath.empty()) {
        int idxToDelete = -1;
        ImGui::Text("Arguments:");
        // add button
        ImGui::SameLine();
        if (ImGui::Button("+")) {
            component.args.emplace_back(ScriptArgument{});
        }

        for (unsigned int i = 0; i < component.args.size(); i++) {
            ImGui::PushID(i);
            ImGui::BeginGroup();
            ImGui::Separator();
            // delete button
            if (ImGui::Button("-")) {
                idxToDelete = i;
            }

            // key selector
            ImGui::SameLine();
            ImGui::SetNextItemWidth(140.0f);
            ImGui::InputText("##key", &component.args[i].key);

            // only show rest if key is valid
            if (!component.args[i].key.empty()) {
                // argument type selector
                static const char* argTypes[] = {"bool", "int", "float", "string", "entity",
                    "component", "none"};
                ImGui::SameLine();
                ImGui::SetNextItemWidth(100.0f);
                if (ImGui::BeginCombo("##type", argTypes[component.args[i].type])) {
                    for (unsigned int j = 0; j < ScriptArgument::ARGTYPE_MAX; j++) {
                        bool isSelected = component.args[i].type == j;
                        if (ImGui::Selectable(argTypes[j], &isSelected)) {
                            component.args[i].type = (ScriptArgument::ArgType)j;
                        }
                    }

                    ImGui::EndCombo();
                }

                // argument value input
                bool b;
                switch (component.args[i].type) {
                case ScriptArgument::BOOL:
                    b = bool(component.args[i].arg._int);
                    if (ImGui::Checkbox("##boolselector", &b)) {
                        component.args[i].arg._int = int(b);
                    }
                    break;
                case ScriptArgument::INT:
                    ImGui::InputInt("##intselector", &component.args[i].arg._int);
                    break;
                case ScriptArgument::FLOAT:
                    ImGui::InputFloat("##floatselector", &component.args[i].arg._float);
                    break;
                case ScriptArgument::STRING:
                    ImGui::InputText("##stringselector", &component.args[i].stringBuf);
                    break;
                case ScriptArgument::ENTITY:
                    drawEntitySelector(component, i);
                    break;
                case ScriptArgument::COMPONENT:
                    drawComponentSelectorOuter(component, i);
                    break;
                default:
                    break;
                }
            }

            if (i == component.args.size() - 1)
                ImGui::Separator();

            ImGui::EndGroup();
            ImGui::PopID();
        }
        if (idxToDelete >= 0) {
            component.args.erase(component.args.begin() + idxToDelete);
        }
    }
    ImGui::BeginDisabled(component.scriptPath.empty());
    if (ImGui::Button("Test")) {
        scene.updateReferences();
        component.start();
        component.update(.016f, scene.selectedEntity->state);
        component.stop();
    }
    ImGui::EndDisabled();
}

void drawComponentProps(CameraComponent& component) {
    // cam properties
    ImGui::InputFloat3("Eye", &component.eye[0]);
    ImGui::InputFloat3("Center", &component.center[0]);
    ImGui::InputFloat3("Up", &component.up[0]);
    ImGui::InputFloat("FOV", &component.fov);

    // active on start
    bool isCamActive = component.uuid == CameraComponent::activeUuid;
    if (ImGui::Checkbox("Default Active", &isCamActive))
        CameraComponent::activeUuid = component.uuid;
}

void drawComponentContextMenu(int i, int& componentToDelete) {
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Add Audio Source Component")) {
            scene.selectedEntity->components.addComponent(AudioSourceComponent());
        }
        if (ImGui::MenuItem("Add Camera Component")) {
            scene.selectedEntity->components.addComponent(CameraComponent());
        }
        if (ImGui::MenuItem("Add Script Component")) {
            scene.selectedEntity->components.addComponent(ScriptComponent());
        }
        if (ImGui::MenuItem("Add Transform Component")) {
            scene.selectedEntity->components.addComponent(TransformComponent());
        }
        if (ImGui::MenuItem("Delete Component")) {
            componentToDelete = i;
        }
        ImGui::EndPopup();
    }
}

template <typename T>

void drawComponentList(std::vector<T>& components) {
    int componentToDelete = -1;

    // draw list
    for (int i = 0; i < components.size(); i++) {
        ImGui::PushID(i);
        if (ImGui::TreeNodeEx(components[i].name.c_str(),
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                    ImGuiTreeNodeFlags_DefaultOpen)) {
            drawComponentContextMenu(i, componentToDelete);

            drawComponentProps(components[i]);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    // handle deletion requests
    if (componentToDelete >= 0) {
        components.erase(components.begin() + componentToDelete);
    }
}

template <>

void drawComponentList(std::vector<CameraComponent>& components) {
    int componentToDelete = -1;

    // draw list
    for (int i = 0; i < components.size(); i++) {
        ImGui::PushID(i);
        if (ImGui::TreeNodeEx(components[i].name.c_str(),
                ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
                    ImGuiTreeNodeFlags_DefaultOpen)) {
            // update selected camera
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
                scene.selectedCameraIdx = i;

            drawComponentContextMenu(i, componentToDelete);

            drawComponentProps(components[i]);
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    // handle deletion requests
    if (componentToDelete >= 0) {
        components.erase(components.begin() + componentToDelete);
        scene.selectedCameraIdx = 0;
    }
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
            ImGui::InputFloat3("Position", &scene.selectedEntity->state.position[0]);
            ImGui::InputFloat4("Rotation", &scene.selectedEntity->state.rotation[0]);
            ImGui::InputFloat3("Scale", &scene.selectedEntity->state.scale[0]);

            ImGui::Separator();
            // component lists
            // AudioSourceComponent
            drawComponentList(scene.selectedEntity->components.vecAudioSourceComponent);

            // CameraComponent
            drawComponentList(scene.selectedEntity->components.vecCameraComponent);

            // ModelComponent
            drawComponentList(scene.selectedEntity->components.vecModelComponent);

            // ScriptComponent
            drawComponentList(scene.selectedEntity->components.vecScriptComponent);

            // SkeletalModelComponent
            drawComponentList(scene.selectedEntity->components.vecSkeletalModelComponent);

            // TransformComponent
            drawComponentList(scene.selectedEntity->components.vecTransformComponent);

            // Universal Context Menu
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
                if (ImGui::MenuItem("Add Camera Component")) {
                    scene.selectedEntity->components.addComponent(CameraComponent());
                }
                if (ImGui::MenuItem("Add Model Component")) {
                    scene.selectedEntity->components.addComponent(ModelComponent());
                }
                if (ImGui::MenuItem("Add Animated Model Component")) {
                    scene.selectedEntity->components.addComponent(SkeletalModelComponent());
                }
                if (ImGui::MenuItem("Add Script Component")) {
                    scene.selectedEntity->components.addComponent(ScriptComponent());
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
    static assetfolder::AssetDescriptor currLevelDir = {"", "",
        assetfolder::AssetDescriptor::EFileType::INVALID};
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

                    glfwSetWindowTitle(baseWindow,
                        (levelDescriptors[i].name + " - ONO Engine").c_str());
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

        // CPU
        ImGui::Text("CPU Usage | Curr: %.3f%%, Min: %.3f%%, Max: %.3f%%",
            metrics::getCurrentCPUUsage(), metrics::getMinCPUUsage(), metrics::getMaxCPUUsage());

        // draw plot
        if (ImPlot::BeginPlot("CPU Usage", ImVec2(400.f, 120.f),
                ImPlotFlags_NoMenus | ImPlotFlags_NoChild | ImPlotFlags_NoInputs |
                    ImPlotFlags_CanvasOnly | ImPlotFlags_NoLegend | ImPlotFlags_NoFrame)) {
            // setup y
            ImPlot::SetupAxis(ImAxis_Y1, "%", ImPlotAxisFlags_AutoFit);
            // hide x axis ticks
            ImPlot::SetupAxis(ImAxis_X1, nullptr,
                ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLineG("CPU Usage", &metrics::getCPUSample, nullptr,
                metrics::getMaxSampleCount(), ImPlotLineFlags_SkipNaN | ImPlotLineFlags_Shaded);
        }
        ImPlot::EndPlot();

        // Physical Mem
        float physMemMB = float(metrics::getCurrentPhysicalMemoryUsage()) / (1024.f * 1024.f);
        float minPhysMemMB = float(metrics::getMinPhysicalMemoryUsage()) / (1024.f * 1024.f);
        float maxPhysMemMB = float(metrics::getMaxPhysicalMemoryUsage()) / (1024.f * 1024.f);
        ImGui::Text("Physical Memory Used | Curr: %.3f MB, Min: %.3f MB, Max: %.3f MB", physMemMB,
            minPhysMemMB, maxPhysMemMB);

        // draw plot
        if (ImPlot::BeginPlot("Phys Mem Usage", ImVec2(400.f, 120.f),
                ImPlotFlags_NoMenus | ImPlotFlags_NoChild | ImPlotFlags_NoInputs |
                    ImPlotFlags_CanvasOnly | ImPlotFlags_NoLegend | ImPlotFlags_NoFrame)) {

            // setup y
            ImPlot::SetupAxis(ImAxis_Y1, "MB", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_LockMin);

            // hide x axis ticks
            ImPlot::SetupAxis(ImAxis_X1, nullptr,
                ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLineG("Phys Mem Usage", &metrics::getPhysMemSample, nullptr,
                metrics::getMaxSampleCount(), ImPlotLineFlags_SkipNaN | ImPlotLineFlags_Shaded);
        }
        ImPlot::EndPlot();

        // Virtual Mem
        float virtMemMB = float(metrics::getCurrentVirtualMemoryUsage()) / (1024.f * 1024.f);
        float minVirtMemMB = float(metrics::getMinVirtualMemoryUsage()) / (1024.f * 1024.f);
        float maxVirtMemMB = float(metrics::getMaxVirtualMemoryUsage()) / (1024.f * 1024.f);
        ImGui::Text("Virtual Memory Used | Curr: %.3f MB, Min: %.3f MB, Max: %.3f MB", virtMemMB,
            minVirtMemMB, maxVirtMemMB);

        // draw plot
        if (ImPlot::BeginPlot("Virt Mem Usage", ImVec2(400.f, 120.f),
                ImPlotFlags_NoMenus | ImPlotFlags_NoChild | ImPlotFlags_NoInputs |
                    ImPlotFlags_CanvasOnly | ImPlotFlags_NoLegend | ImPlotFlags_NoFrame)) {
            // setup y
            ImPlot::SetupAxis(ImAxis_Y1, "MB", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_LockMin);
            // hide x axis ticks
            ImPlot::SetupAxis(ImAxis_X1, nullptr,
                ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_AutoFit);
            ImPlot::PlotLineG("Virt Mem Usage", &metrics::getVirtMemSample, nullptr,
                metrics::getMaxSampleCount(), ImPlotLineFlags_SkipNaN | ImPlotLineFlags_Shaded);
        }
        ImPlot::EndPlot();

        // Audio stuff
        ImGui::Text("AUDIO: %i clips loaded", audio::getAudioClipCount());

        // Texture stuff
        ImGui::Text("TEXTURES %i loaded", getTextureCount());

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

    // ImGui::ShowDemoWindow();

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
