
//imagui file for collision.
//stand alone.
#include <iostream>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <GLFW/glfw3.h> 
#include <windows.h> 
#include <vector>
#include <string>
#include "imgui_node_editor.h"

//glfw callback
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static std::vector<std::string> presetNames;
const char* presetCharArray[150];
static int position = 0;
//static file opener
void static loadPresetFile()
{
    //the following code for opening a file is done using win32api link: https://learn.microsoft.com/en-us/windows/win32/dlgbox/using-common-dialog-boxes
    OPENFILENAME ofn;       // common dialog box structure
    const size_t string_size = 260;
    char file_name[string_size];       // buffer for file name
    memset(file_name, '\0', string_size);
    const char* filePtr = file_name;
    HANDLE hf;              // file handle

    //change from unicode to widechar.
    //this will crash for large file names. fix later.
    //will not work for double byte.
    LPSTR ptr = file_name;

    std::string fileName;
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFile = ptr;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(file_name);
    ofn.lpstrFilter = "Preset file(JSON) (*.json)\0*.json\0All (*.*)\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select a Preset file";
    ofn.lpstrDefExt = "json";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

    bool status = GetOpenFileName(&ofn);

   



    if (status)
    {
        std::string presetName = std::string(filePtr);
        presetNames.push_back(presetName);
        presetCharArray[position++] = filePtr;
    }
}


void static deletePresetFile()
{
  
}



int main()
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    //add versions here.
    //to do later
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);


    //glfw initial setup. to be removed.
    GLFWwindow* window = glfwCreateWindow(1280, 720, "GamePlay widgets", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync


    //imgui initialization.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    //fonts to be merged from main code.
    bool show_demo_window = true;
    bool show_another_window = true;
    bool show_collision_window = true;
    bool show_animation_path = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    static ImGuiComboFlags flags = 0;


    while (!glfwWindowShouldClose(window))

    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        

        //My code
        if (show_collision_window)
        {
            ImGui::Begin("Collision Window", &show_collision_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::CheckboxFlags("Collision Enabled", &flags, ImGuiComboFlags_PopupAlignLeft);
            unsigned int currentPos = 0;
            ImGui::Text("Add custom presets: ");
            if (ImGui::Button("Add Preset.."))
                loadPresetFile();
            //replace with Martins file opener.
            //windows file open

            //combo preview values.
            int prevValue = 0;
            
            if (position > 0)
            {
                const char* combo_preview_value = presetNames[prevValue].c_str();  // Pass in the preview value visible before opening the combo (it could be anything)
                std::string test = std::string(combo_preview_value);
                if (ImGui::BeginCombo("Custom Presets", combo_preview_value, flags))
                {
                    for (int i = 0; i < position; i++)
                    {
                        const bool is_selected = (prevValue == i);
                        std::string curr = presetNames[i];
                        if (ImGui::Selectable(curr.c_str(), is_selected))
                            prevValue = i;

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                if (ImGui::Button("Delete Preset"))
                    deletePresetFile();
            }




            ImGui::Text(" ");
            ImGui::Text(" ");
            ImGui::Text("Object Type");
            const char* objList[] = { "World Static", "World Dynamic", "Pawn", "Destructible"};
            static int currPos = 0; // Here we store our selection data as an index.
            const char* combo_preview = objList[currPos];  // Pass in the preview value visible before opening the combo (it could be anything)
            if (ImGui::BeginCombo(" ", combo_preview, flags))
            {
                for (int i = 0; i < 4; ++i)
                {
                    const bool is_selected = (currPos == i);
                    if (ImGui::Selectable(objList[i], is_selected))
                        currPos = i;

                    // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            ImGui::Text("Collision Responses");
            ImGui::Text("Object Responses");
            ImGui::Text(" ");
            ImGui::Text("Ignore   Overlap   Block ");
            
            const char* names[3] = { "      ", "     ", "World Static"};
            const char* names1[3] = { "      ", "     ", "World Dynamic" };
            const char* names2[3] = { "      ", "     ", "World Pawn" };
            const char* names3[3] = { "      ", "     ", "World Destructible" };
            static bool opened[3] = { true, true, true}; // Persistent user state
            for (int n = 0; n < 3; n++)
            {
                if (n > 0) { ImGui::SameLine(); }
                ImGui::Checkbox(names[n], &opened[n]);
            }
            for (int n = 0; n < 3; n++)
            {
                if (n > 0) { ImGui::SameLine(); }
                ImGui::Checkbox(names1[n], &opened[n]);
            }
            for (int n = 0; n < 3; n++)
            {
                if (n > 0) { ImGui::SameLine(); }
                ImGui::Checkbox(names2[n], &opened[n]);
            }
            for (int n = 0; n < 3; n++)
            {
                if (n > 0) { ImGui::SameLine(); }
                ImGui::Checkbox(names3[n], &opened[n]);
            }

            //windows file close


            ImGui::End();
        }


        //my code end




       
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }



	return 0;
}