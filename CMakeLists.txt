cmake_minimum_required(VERSION 3.9)
set(CXX_STANDARD 11)
project(BuildSys CXX C)

#define as engine
add_compile_definitions(ONO_ENGINE_ONLY)
add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

#common vars
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

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
  "${IMGUIDIR}/misc/cpp/imgui_stdlib.cpp"
)

#setup ImPlot
include_directories(${PROJECT_NAME} "external/implot")
set(IMPLOTSRC "external/implot/implot.cpp" 
    "external/implot/implot_items.cpp" "external/implot/implot_demo.cpp")

#setup lua interpreter
set(LUADIR "external/lua-5.4.4/src")
include_directories(${LUADIR})
aux_source_directory("${LUADIR}/" LUASRC)

# # Import the ReactPhysics3D library 
# find_package(ReactPhysics3D REQUIRED) 


#setup icon on windows
if(WIN32)
    set(ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/appicon.rc")
endif(WIN32)

#setup executables
add_executable(
    ${PROJECT_NAME} src/main.cpp external/ImGuizmo/ImGuizmo.cpp
    external/tinyfiledialogs/tinyfiledialogs.c ${IMGUICORE} ${IMGUIBACKENDS}
    ${IMPLOTSRC} ${LUASRC} src/asset_import/audio.cpp src/asset_import/images.cpp
    src/asset_import/folders.cpp src/fdutil.cpp src/levels.cpp
    src/model_import/model.cpp src/model_import/mesh.cpp
    src/logging.cpp src/gui.cpp src/scripting.cpp src/metrics.cpp
    src/ECS/Component/BaseComponent.cpp
    src/ECS/Component/AudioSourceComponent.cpp src/ECS/Component/CameraComponent.cpp
    src/ECS/Component/TransformComponent.cpp src/ECS/Component/ScriptComponent.cpp
    src/ECS/Component/ModelComponent.cpp src/ECS/Component/SkyBoxComponent.cpp src/ECS/Component/LightComponent.cpp
    src/ECS/Component/ControllerComponent.cpp src/ECS/Component/PlayerControllerComponent.cpp 
    src/ECS/Entity/BaseEntity.cpp src/ECS/Entity/CameraEntity.cpp
    src/ECS/Entity/ModelEntity.cpp src/ECS/Entity/SkeletalMeshEntity.cpp
    src/ECS/EntityState/EntityState.cpp 
    src/ECS/System/InputSystem.cpp
    src/ECS/Scene/Scene.cpp  src/ECS/ComponentStorage/ComponentStorage.cpp
    render-engine/RenderManager.cpp render-engine/RenderManager.h render-engine/RenderPipeline.h
    render-engine/RenderPipeline.cpp render-engine/Camera.h render-engine/Camera.cpp
    render-engine/Buffer.h render-engine/Buffer.cpp
 	src/asset_import/model.cpp src/asset_import/materials.cpp
	render-engine/LightSource.h render-engine/LightSource.cpp
	src/ECS/Component/SkeletalModelComponent.cpp src/model_import/animation.cpp
    src/model_import/bone.cpp src/asset_import/animation.cpp
    ${ICON_RESOURCE}
    src/physics_engine/physicsEngine.cpp src/ECS/Component/RigidBodyComponent.cpp 
    src/physics_engine/PhysicsEvents.cpp)

#add glfw
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
add_subdirectory("external/glfw-3.3.6")

#add glew
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory("external/glew-2.1.0/build/cmake")

#add glm
add_subdirectory("external/glm")

#add spdlog
add_subdirectory("external/spdlog")

#add ReactPhysics3D
add_subdirectory("external/reactphysics3d")

#setup linker
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/glew-2.1.0/include
    ${PROJECT_SOURCE_DIR}/external/soloud/include ${PROJECT_SOURCE_DIR}/external/rapidjson
    ${PROJECT_SOURCE_DIR}/external/reactphysics3d/include
)
target_link_libraries(${PROJECT_NAME} glfw opengl32 glew_s glm assimp soloud spdlog reactphysics3d)

#generate ComponentStorage code if necessary
add_custom_target(ComponentStorageGen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/build-util/lua.exe 
        ${CMAKE_CURRENT_SOURCE_DIR}/build-util/gen_component_storage.lua
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/build-util/gen_component_storage.lua
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/ECS/Component/component_list.txt
)
add_dependencies(${PROJECT_NAME} ComponentStorageGen)

#create template
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    BYPRODUCTS ${CMAKE_CURRENT_SOURCE_DIR}/template
    #copy over most engine source files
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/asset_import
    ${CMAKE_CURRENT_SOURCE_DIR}/template/src/asset_import

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/ECS
    ${CMAKE_CURRENT_SOURCE_DIR}/template/src/ECS

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/model_import
    ${CMAKE_CURRENT_SOURCE_DIR}/template/src/model_import

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/physics_engine
    ${CMAKE_CURRENT_SOURCE_DIR}/template/src/physics_engine

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/render-engine
    ${CMAKE_CURRENT_SOURCE_DIR}/template/render-engine

    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/build-util
    ${CMAKE_CURRENT_SOURCE_DIR}/template/build-util


    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fdutil.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fdutil.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/levels.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/levels.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/logging.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/scripting.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/util.h
    ${CMAKE_CURRENT_SOURCE_DIR}/template/src

    #shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets/shaders
    ${CMAKE_CURRENT_SOURCE_DIR}/template/assets/shaders

    #install assimp
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}/external/assimp-5.2.5
    --config $<$<CONFIG:Debug>:Debug> $<$<CONFIG:RelWithDebInfo>:RelWithDebInfo> 
    $<$<CONFIG:Release>:Release>
    --prefix ${CMAKE_CURRENT_SOURCE_DIR}/template/external/assimp

    #install glfw
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}/external/glfw-3.3.6
    --config $<$<CONFIG:Debug>:Debug> $<$<CONFIG:RelWithDebInfo>:RelWithDebInfo> 
    $<$<CONFIG:Release>:Release>
    --prefix ${CMAKE_CURRENT_SOURCE_DIR}/template/external/glfw-3.3.6

    #install soloud
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}/external/soloud/contrib
    --config $<$<CONFIG:Debug>:Debug> $<$<CONFIG:RelWithDebInfo>:RelWithDebInfo> 
    $<$<CONFIG:Release>:Release>
    --prefix ${CMAKE_CURRENT_SOURCE_DIR}/template/external/soloud

    #install reactphysics3d
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR}/external/reactphysics3d
    --config $<$<CONFIG:Debug>:Debug> $<$<CONFIG:RelWithDebInfo>:RelWithDebInfo> 
    $<$<CONFIG:Release>:Release>
    --prefix ${CMAKE_CURRENT_SOURCE_DIR}/template/external/reactphysics3d


    #copy template
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    ${CMAKE_CURRENT_SOURCE_DIR}/template
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/template
)

#copy dirs after build
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
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/logo.ico
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/Launcher/logo.png
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
