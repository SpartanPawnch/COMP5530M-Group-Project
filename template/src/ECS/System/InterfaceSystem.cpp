#include "InterfaceSystem.h"

InterfaceSystem::InterfaceSystem(Scene* sc) {
    scene = sc;
    start();
}

InterfaceSystem::~InterfaceSystem() {
    stop();
}

void InterfaceSystem::start() {
    glfwInit();
    glfwWindowHint(GLFW_MAXIMIZED, 1);
    glfwWindowHint(GLFW_SAMPLES, 4);
    // create window
    window = glfwCreateWindow(1600, 900, "Engine", NULL, NULL);
    glfwMakeContextCurrent(window);

    // setup OpenGL
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cout << "Error: " << glewGetErrorString(err) << "\n";
        return;
    }

    // swap buffers to avoid the window not responding
    glClearColor(.7f, .7f, .7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();

    // Init ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
}

void InterfaceSystem::update(float dt) {
}

void InterfaceSystem::stop() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();
}
