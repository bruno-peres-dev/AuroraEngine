add_executable(AuroraRuntime
    src/main.cpp
)

target_include_directories(AuroraRuntime PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_link_libraries(AuroraRuntime PRIVATE aurora_core aurora_platform aurora_rhi)

set_target_properties(AuroraRuntime PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


