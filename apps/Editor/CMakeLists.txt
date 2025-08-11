include(FetchContent)

FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG docking
)
FetchContent_MakeAvailable(imgui)

add_library(imgui_backend STATIC
  ${imgui_SOURCE_DIR}/imgui.cpp
  ${imgui_SOURCE_DIR}/imgui_draw.cpp
  ${imgui_SOURCE_DIR}/imgui_tables.cpp
  ${imgui_SOURCE_DIR}/imgui_widgets.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp
  ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)
target_include_directories(imgui_backend PUBLIC ${imgui_SOURCE_DIR} ${imgui_SOURCE_DIR}/backends)
target_compile_definitions(imgui_backend PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)

add_executable(AuroraEditor
    src/main.cpp
    src/EditorApp.cpp
    src/EditorApp.hpp
    src/CameraSystem.cpp
    src/CameraSystem.hpp
    src/RenderSystem.cpp
    src/RenderSystem.hpp
)

target_include_directories(AuroraEditor PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_SOURCE_DIR}/engine/rhi/src/OpenGL)

target_link_libraries(AuroraEditor PRIVATE aurora_core aurora_platform aurora_rhi aurora_assets imgui_backend)

# GLM (header-only)
FetchContent_Declare(
  glm
  GIT_REPOSITORY https://github.com/g-truc/glm.git
  GIT_TAG 1.0.1
)
FetchContent_MakeAvailable(glm)
target_include_directories(AuroraEditor PRIVATE ${glm_SOURCE_DIR})

set_target_properties(AuroraEditor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Copiar shaders e assets para a pasta do executável
add_custom_command(TARGET AuroraEditor POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:AuroraEditor>/shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:AuroraEditor>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/viewport.vert.glsl"
            "$<TARGET_FILE_DIR:AuroraEditor>/shaders/viewport.vert.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/viewport.frag.glsl"
            "$<TARGET_FILE_DIR:AuroraEditor>/shaders/viewport.frag.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/apps/Runtime/shaders/texture.vert.glsl"
            "$<TARGET_FILE_DIR:AuroraEditor>/shaders/texture.vert.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/apps/Runtime/shaders/texture.frag.glsl"
            "$<TARGET_FILE_DIR:AuroraEditor>/shaders/texture.frag.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/apps/Runtime/assets/checker.ppm"
            "$<TARGET_FILE_DIR:AuroraEditor>/assets/checker.ppm"
)


