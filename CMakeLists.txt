cmake_minimum_required(VERSION 3.9)
set(CXX_STANDARD 11)
project(BuildSys CXX C)

#setup SoLoud
set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE)
set(SOLOUD_BACKEND_WASAPI ON CACHE BOOL "" FORCE)
add_subdirectory("external/soloud/contrib")

#setup assimp
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
add_subdirectory("external/assimp-5.2.5")

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
    src/asset_import/folders.cpp src/fdutil.cpp src/levels.cpp
    src/model_import/model.cpp src/model_import/mesh.cpp
    src/logging.cpp src/gui.cpp
    src/ECS/engine.cpp src/ECS/Component/BaseComponent.cpp
    src/ECS/Component/TransformComponent.cpp
    src/ECS/Entity/BaseEntity.cpp src/ECS/Entity/CameraEntity.cpp
    src/ECS/Entity/ModelEntity.cpp src/ECS/Entity/SkeletalMeshEntity.cpp
    src/ECS/Scene/Scene.cpp  src/ECS/System/BaseSystem.cpp 
    src/ECS/System/InterfaceSystem.cpp
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

#add spdlog
add_subdirectory("external/spdlog")

#setup linker
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/glew-2.1.0/include
  ${PROJECT_SOURCE_DIR}/external/soloud/include ${PROJECT_SOURCE_DIR}/external/rapidjson
)
target_link_libraries(${PROJECT_NAME} glfw opengl32 glew_s glm assimp soloud spdlog)
add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/template
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/template)

#copy assets after build
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/assets/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/fonts/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts
)