add_executable(AuroraRuntime
    src/main.cpp
    src/Application.cpp
    src/Application.hpp
)

target_include_directories(AuroraRuntime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(AuroraRuntime PRIVATE aurora_core aurora_platform aurora_rhi aurora_assets)

set_target_properties(AuroraRuntime PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

# Copiar shaders para a pasta do executável para facilitar execução fora do Visual Studio
add_custom_command(TARGET AuroraRuntime POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:AuroraRuntime>/shaders"
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:AuroraRuntime>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.vert.glsl"
            "$<TARGET_FILE_DIR:AuroraRuntime>/shaders/triangle.vert.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/triangle.frag.glsl"
            "$<TARGET_FILE_DIR:AuroraRuntime>/shaders/triangle.frag.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/texture.vert.glsl"
            "$<TARGET_FILE_DIR:AuroraRuntime>/shaders/texture.vert.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/shaders/texture.frag.glsl"
            "$<TARGET_FILE_DIR:AuroraRuntime>/shaders/texture.frag.glsl"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/assets/checker.ppm"
            "$<TARGET_FILE_DIR:AuroraRuntime>/assets/checker.ppm"
)


