#include "InterfaceSystem.h"

InterfaceSystem::InterfaceSystem(Scene* sc)
{
    scene = sc;
    start();
}

InterfaceSystem::~InterfaceSystem()
{
    stop();
}

void InterfaceSystem::start()
{
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //create window
    window = glfwCreateWindow(1600, 900, "Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    //setup OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(err) << "\n";
        return;
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
}

void InterfaceSystem::update(float dt)
{
    // get window dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    //prepare gui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 mousePos = ImGui::GetMousePos();

    // --- Get Gui Input ---
    if (ImGui::Begin("Entities", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        for (int i = 0; i < scene->entities.size(); i++)
        {
            if (ImGui::TreeNodeEx(scene->entities.at(i).name.c_str(), ImGuiTreeNodeFlags_DefaultOpen, scene->entities.at(i).name.c_str())) {
                if (ImGui::IsItemClicked()) {
                    if (scene->selectedEntity != &scene->entities.at(i)) {
                        scene->selectedEntity = &scene->entities.at(i);
                    }
                }
                //TODO:display children if open
                ImGui::TreePop();
            }
        }
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Add Entity")) {
                scene->addEntity(BaseEntity());
            }
            if (ImGui::MenuItem("Add Camera Entity")) {
                scene->addEntity(CameraEntity());
            }
            if (ImGui::MenuItem("Add Model Entity")) {
                scene->addEntity(ModelEntity());
            }
            if (ImGui::MenuItem("Add Skeletal Mesh Entity")) {
                scene->addEntity(SkeletalMeshEntity());
            }
            ImGui::EndPopup();
        }
        ImGui::End();
    }

    if (ImGui::Begin("Components", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
        if (scene->selectedEntity == nullptr) {
            ImGui::Text("Select an Entity to show it's components");
        } else {
            for (int i = 0; i < scene->selectedEntity->components.size(); i++)
            {
                if (ImGui::TreeNodeEx(scene->selectedEntity->components.at(i)->name.c_str())) {
                    ImGui::TreePop();
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

void InterfaceSystem::stop()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}