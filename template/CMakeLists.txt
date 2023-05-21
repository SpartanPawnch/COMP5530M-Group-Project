cmake_minimum_required(VERSION 3.9)
set(CXX_STANDARD 11)
project(BuildTest CXX C)

if(MSVC)
add_definitions(/W0)
else()
add_definitions(-w)
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

#setup SoLoud
#set(SOLOUD_BACKEND_SDL2 OFF CACHE BOOL "" FORCE)
#set(SOLOUD_BACKEND_WASAPI ON CACHE BOOL "" FORCE)
#add_subdirectory("external/soloud/contrib")
find_package(soloud CONFIG REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/external/soloud/cmake)

#setup assimp
#set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
#set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)
#add_subdirectory("external/assimp-5.2.5")
#include_directories(external/assimp-5.2.5/include)
find_package(assimp CONFIG REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/external/assimp/lib/cmake/assimp-5.2)

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

find_package(ReactPhysics3D CONFIG REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/external/reactphysics3d/lib/cmake/ReactPhysics3D)

#setup executables
add_executable(
    ${PROJECT_NAME} src/main.cpp 
    external/tinyfiledialogs/tinyfiledialogs.c ${IMGUICORE} ${IMGUIBACKENDS}
    ${LUASRC} ${IMPLOTSRC} src/asset_import/audio.cpp src/asset_import/images.cpp
    src/asset_import/folders.cpp src/fdutil.cpp src/levels.cpp
    src/model_import/model.cpp src/model_import/mesh.cpp
    src/logging.cpp src/scripting.cpp src/metrics.cpp src/hud.cpp
    src/ECS/Component/BaseComponent.cpp
    src/ECS/Component/AudioSourceComponent.cpp src/ECS/Component/CameraComponent.cpp
    src/ECS/Component/TransformComponent.cpp src/ECS/Component/ScriptComponent.cpp src/ECS/Component/ModelComponent.cpp
    src/ECS/Component/LightComponent.cpp src/ECS/Component/SkyboxComponent.cpp
    src/ECS/Component/ControllerComponent.cpp src/ECS/Component/PlayerControllerComponent.cpp 
	src/ECS/EntityState/EntityState.cpp
    src/ECS/Entity/BaseEntity.cpp src/ECS/Entity/CameraEntity.cpp
    src/ECS/Entity/ModelEntity.cpp src/ECS/Entity/SkeletalMeshEntity.cpp
    src/ECS/Scene/Scene.cpp  src/ECS/ComponentStorage/ComponentStorage.cpp
    src/ECS/System/InputSystem.cpp src/ECS/System/InputSystem.h
    render-engine/RenderManager.cpp render-engine/RenderManager.h render-engine/RenderPipeline.h
    render-engine/RenderPipeline.cpp render-engine/Camera.h render-engine/Camera.cpp
    render-engine/Buffer.h render-engine/Buffer.cpp
 	src/asset_import/model.cpp
	render-engine/LightSource.h render-engine/LightSource.cpp
	src/ECS/Component/SkeletalModelComponent.cpp src/model_import/animation.cpp 
	src/ECS/Component/DirectionalLightComponent.cpp
	src/model_import/bone.cpp src/asset_import/animation.cpp
    src/asset_import/materials.cpp src/asset_import/materials.h
    src/physics_engine/physicsEngine.cpp src/ECS/Component/RigidBodyComponent.cpp 
    src/physics_engine/PhysicsEvents.cpp
)

#add glfw
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(USE_MSVC_RUNTIME_LIBRARY_DLL OFF CACHE BOOL "" FORCE)
#add_subdirectory("external/glfw-3.3.6")
#include_directories(external/glfw-3.3.6/include)
find_package(glfw3 CONFIG REQUIRED PATHS ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw-3.3.6/lib/cmake/glfw3)

#add glew
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
add_subdirectory("external/glew-2.1.0/build/cmake")

#add glm
add_subdirectory("external/glm")

#add spdlog
add_subdirectory("external/spdlog")

#setup linker
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/external/glew-2.1.0/include
    ${PROJECT_SOURCE_DIR}/external/soloud/include/soloud ${PROJECT_SOURCE_DIR}/external/rapidjson
	${ASSIMP_INCLUDE_DIRS} glfw
)

target_link_libraries(${PROJECT_NAME} glfw opengl32 glew_s glm ${ASSIMP_LIBRARIES} soloud spdlog 
    ReactPhysics3D::ReactPhysics3D)

#generate ComponentStorage code if necessary
add_custom_target(ComponentStorageGen
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/build-util/lua.exe 
        ${CMAKE_CURRENT_SOURCE_DIR}/build-util/gen_component_storage.lua
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/build-util/gen_component_storage.lua
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/ECS/Component/component_list.txt
)
add_dependencies(${PROJECT_NAME} ComponentStorageGen)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/levels/
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/levels
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/project.json
    $<TARGET_FILE_DIR:${PROJECT_NAME}>
)
