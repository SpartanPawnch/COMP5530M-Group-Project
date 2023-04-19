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
#include "ECS/Entity/CameraEntity.h"
#include "ECS/Entity/ModelEntity.h"
#include "ECS/Entity/SkeletalMeshEntity.h"
#include "ECS/Scene/Scene.h"

using namespace ImGui;

// --- GUI constants, possibly replace with settings file ---
namespace guicfg {
    const ImVec2 assetMgrPadding(10.0f, 25.0f);
    const ImVec2 assetMgrIconSize(60.0f, 60.0f);
    const ImVec2 assetMgrItemSize(80.0f, 80.0f);
    const float splitterThickness = 7.f;
    const ImGuiID dockspaceID = 1;

    const char* titleFontPath = "fonts/Phudu-Medium.ttf";
    const char* regularFontPath = "fonts/Armata-Regular.ttf";

    ImFont* titleFont = nullptr;
    ImFont* regularFont = nullptr;
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

static Scene scene;

GLuint viewportFramebuffer;
static GLuint viewportTex;

//TODO Load/Save style to disk
static ImGuiStyle guiStyle;

GUIManager::GUIManager(GLFWwindow* window) {
    //Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //enable docking
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    //load fonts
    ImFont* defaultFont = io.Fonts->AddFontDefault();
    guicfg::titleFont = io.Fonts->AddFontFromFileTTF(guicfg::titleFontPath, 16.f);
    guicfg::regularFont = io.Fonts->AddFontFromFileTTF(guicfg::regularFontPath, 15.f);
    io.Fonts->Build();

    //fallback to default if loading fails
    guicfg::titleFont = guicfg::titleFont ? guicfg::titleFont : defaultFont;
    guicfg::regularFont = guicfg::regularFont ? guicfg::regularFont : defaultFont;

    io.FontDefault = guicfg::titleFont;

    //set UI colors
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.23f, 0.26f, 0.94f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.06f, 0.21f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.11f, 0.58f, 0.62f, 1.00f);
    colors[ImGuiCol_Tab] = ImVec4(0.13f, 0.38f, 0.42f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.23f, 0.82f, 0.84f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.82f, 0.84f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.38f, 0.42f, 0.86f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.71f, 0.73f, 1.00f);

    //load ui elements
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
    if (projectThread.joinable())
        projectThread.join();

    glDeleteFramebuffers(1, &viewportFramebuffer);
    glDeleteTextures(1, &viewportTex);

}

inline void drawAudioDemo() {
    if (ImGui::Begin("Audio Demo", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::PushFont(guicfg::regularFont);
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
        ImGui::PopFont();
    }
    ImGui::End();
}

inline float drawMainMenu(const char* executablePath) {
    float barHeight = .0f;
    if (ImGui::BeginMainMenuBar()) {
        // ImGui::Text("Active Directory: %s", activePath.c_str());

        //file menu
        if (ImGui::BeginMenu("File##mainmenu")) {
            ImGui::PushFont(guicfg::regularFont);
            if (ImGui::MenuItem("Create Project")) {
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
            if (ImGui::MenuItem("Open Project")) {
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
            ImGui::PopFont();
            ImGui::EndMenu();
        }

        //build menu
        if (ImGui::BeginMenu("Build##mainmenu")) {
            ImGui::PushFont(guicfg::regularFont);
            if (ImGui::MenuItem("Build and Run")) {
                if (!activePath.empty()) {
                    //wait for current op to finish
                    if (projectThread.joinable())
                        projectThread.join();

                    projectThread = std::thread(buildRunProj, activePath,
                        executablePath);

                }
            }
            ImGui::PopFont();
            ImGui::EndMenu();
        }

        if (ImGui::Begin("##FullscreenWindow", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                         ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {
            ImGui::Button("Game Editor");
            ImGui::SameLine();
            ImGui::Button("UI Editor");
            ImGui::SameLine();
            ImGui::Button("Character Editor");
            ImGui::SameLine();
            ImGui::Button("World Editor");
            ImGui::End();
        }

        //get height
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
            //wait for current op to finish
            if (!path.empty()) {
                model.loadModel(path);
            }
        }
        ImGui::Text("Model Meshes: %d", model.meshes.size());
        ImGui::Text("Model Textures: %d", model.textures_loaded.size());
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
        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawAssetBrowser(GLFWwindow* window) {
    //folder debug
    if (ImGui::Begin("Asset Browser", NULL)) {
        ImGui::PushFont(guicfg::regularFont);
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
        for (unsigned int i = 0; i < scene.entities.size(); i++) {
            if (ImGui::TreeNodeEx(scene.entities.at(i).name.c_str(),
                ImGuiTreeNodeFlags_DefaultOpen, scene.entities[i].name.c_str())) {
                if (ImGui::IsItemClicked()) {
                    if (scene.selectedEntity != &scene.entities[i]) {
                        scene.selectedEntity = &scene.entities[i];
                    }
                }
                //TODO:display children if open
                ImGui::TreePop();
            }
        }


        //draw invisible button to allow context menu for full window
        //TODO - adjust size and pos when entity children are implemented
        ImGui::SetCursorPos(ImVec2(.0f, .0f));
        ImVec2 buttonSize = ImGui::GetWindowSize();
        float borderSize = ImGui::GetStyle().WindowBorderSize;
        ImVec2 padding = ImGui::GetStyle().WindowPadding;
        buttonSize.x -= borderSize + 2 * padding.x;
        buttonSize.y -= borderSize + 2 * padding.y;

        ImGui::InvisibleButton("##entitiesinvisblebutton", buttonSize);

        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Add Entity")) {
                scene.addEntity(BaseEntity());
            }
            if (ImGui::MenuItem("Add Camera Entity")) {
                scene.addEntity(CameraEntity());
            }
            if (ImGui::MenuItem("Add Model Entity")) {
                scene.addEntity(ModelEntity());
            }
            if (ImGui::MenuItem("Add Skeletal Mesh Entity")) {
                scene.addEntity(SkeletalMeshEntity());
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

inline void drawProperties() {
    if (ImGui::Begin("Properties")) {
        ImGui::PushFont(guicfg::regularFont);
        //TODO properties for other item types
        if (scene.selectedEntity == nullptr) {
            ImGui::Text("Select an Entity to show it's components");
        }
        else {
            for (unsigned int i = 0; i < scene.selectedEntity->components.size(); i++) {
                if (ImGui::TreeNodeEx(scene.selectedEntity->components[i]->name.c_str())) {
                    //already running into polymorphism caveats
                    if (TransformComponent* transform = dynamic_cast<TransformComponent*>
                        (scene.selectedEntity->components[i])) {
                        drawComponentProps(*transform);
                    }

                    ImGui::TreePop();
                }
            }
        }
        ImGui::PopFont();
    }
    ImGui::End();
}

void prepUI(GLFWwindow* window, const char* executablePath, float dt,
    int viewportWidth, int viewportHeight) {
    ImVec2 windowSize = ImVec2(float(viewportWidth), float(viewportHeight));
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 mousePos = ImGui::GetMousePos();

    //adjust fullscreen windows size to account for menubar
    float mainMenuHeight = drawMainMenu(executablePath);
    windowSize.y -= mainMenuHeight;



    static ImGuiID dockCenter = 0;
    static ImGuiID dockLeft;
    static ImGuiID dockRight;
    static ImGuiID dockBotRight;
    static ImGuiID dockBotLeft;

    //TODO save and load dock state
    //create layout if not present already
    ImGui::SetNextWindowPos(ImVec2(.0f, mainMenuHeight));
    ImGui::SetNextWindowSize(ImVec2(windowSize.x,windowSize.y));
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

            //split vertically
            dockBotRight = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Down, .4f, NULL, &dockCenter);
            dockBotLeft = ImGui::DockBuilderSplitNode(dockBotRight, ImGuiDir_Left, .5f, NULL, &dockBotRight);

            //split horizontally twice
            dockLeft = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Left, .25f, NULL, &dockCenter);
            dockRight = ImGui::DockBuilderSplitNode(dockCenter, ImGuiDir_Right, .25f, NULL, &dockCenter);

            ImGui::DockBuilderFinish(dockId);
            dockSpaceInit = true;
        }
    }

    ImGui::End();

    //TODO undocked windows in front

    ImGui::SetNextWindowDockID(dockLeft, ImGuiCond_Once);
    drawEntities();

    ImGui::SetNextWindowDockID(dockBotLeft, ImGuiCond_Once);
    drawAssetBrowser(window);

    ImGui::SetNextWindowDockID(dockBotRight, ImGuiCond_Once);
    drawLog();

    ImGui::SetNextWindowDockID(dockCenter, ImGuiCond_Once);
    drawViewport();

    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Once);
    drawStyleEditor();

    ImGui::SetNextWindowDockID(dockRight, ImGuiCond_Once);
    drawProperties();

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