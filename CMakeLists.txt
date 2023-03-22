cmake_minimum_required(VERSION 3.9)
set(CXX_STANDARD 11)
project(BuildSys CXX C)

#setup SoLoud
set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE)
set(SOLOUD_BACKEND_WASAPI ON CACHE BOOL "" FORCE)
add_subdirectory("external/soloud/contrib")

#setup Imgui
set(IMGUIDIR "external/imgui")
include_directories(${IMGUIDIR} ${IMGUIDIR}/backends)
aux_source_directory("${IMGUIDIR}/" IMGUICORE)
set(IMGUIBACKENDS "${IMGUIDIR}/backends/imgui_impl_opengl3.cpp" 
  "${IMGUIDIR}/backends/imgui_impl_glfw.cpp"
)

#setup executables
add_executable(
    ${PROJECT_NAME} src/main.cpp src/drawing.cpp 
    external/tinyfiledialogs/tinyfiledialogs.c ${IMGUICORE} ${IMGUIBACKENDS}
    src/asset_import/audio.cpp src/asset_import/images.cpp
)

#add glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory("external/glfw-3.3.6")

#add glew
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory("external/glew-2.1.0/build/cmake")

#add glm
add_subdirectory("external/glm")

#setup linker
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/glew-2.1.0/include
  ${PROJECT_SOURCE_DIR}/external/soloud/include
)
target_link_libraries(${PROJECT_NAME} glfw opengl32 glew_s glm soloud)
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/template
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/template)