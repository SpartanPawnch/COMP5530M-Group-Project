#include "hud.h"
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <imgui_internal.h>
#include <implot.h>

#include "logging.h"
#include "metrics.h"
#include "asset_import/audio.h"
#include "asset_import/images.h"
#include "asset_import/folders.h"
#include "asset_import/animation.h"
#include "asset_import/materials.h"
#include "model_import/model.h"
#include "../render-engine/RenderManager.h"

static RenderManager* renderManager;

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
    //
    // baseWindow = window;
    // materialSystem = MaterialSystem::getInstance();
    renderManager = RenderManager::getInstance();
    //
    // inputSystem = InputSystem::getInstance();
    // physicsEngine = PhysicsEngine::getInstance();
}
GUIManager::~GUIManager() {
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}

void drawStats() {
    if (ImGui::Begin("Statistics", nullptr, ImGuiWindowFlags_NoBackground)) {
        ImGui::PushFont(guicfg::regularFont);

        // FPS
        ImGui::Text("FPS: %.3f | Frametime: %.3f ms", ImGui::GetIO().Framerate,
            renderManager->deltaTime * 1000.f);

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
            ImPlot::EndPlot();
        }

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
            ImPlot::EndPlot();
        }

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
            ImPlot::EndPlot();
        }

        // Audio stuff
        ImGui::Text("AUDIO: %i clips loaded", audio::getAudioClipCount());

        // Texture stuff
        ImGui::Text("TEXTURES %i loaded", getTextureCount());
        ImGui::Text("Point lights: %zu", renderManager->getLightCount());

        ImGui::PopFont();
    }
    ImGui::End();
}

inline void drawLog() {
    if (ImGui::Begin("Log", nullptr, ImGuiWindowFlags_NoBackground)) {
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

void prepUI(int width, int height) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(.0f, .0f));
    ImGui::SetNextWindowSize(ImVec2(400.f, height));
    drawStats();

    ImGui::SetNextWindowPos(ImVec2(float(width) - 400.0f, .0f));
    ImGui::SetNextWindowSize(ImVec2(400.f, height));
    drawLog();

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
