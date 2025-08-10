add_library(aurora_rhi STATIC
    src/RHI.cpp
    src/Null/NullDevice.cpp
    src/OpenGL/GLDevice.cpp
    src/OpenGL/WGLContext.cpp
)

target_include_directories(aurora_rhi PUBLIC include)

target_link_libraries(aurora_rhi PUBLIC aurora_core aurora_platform)
include(FetchContent)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
FetchContent_Declare(
  glad
  GIT_REPOSITORY https://github.com/Dav1dde/glad.git
  GIT_TAG v0.1.36
)
FetchContent_MakeAvailable(glad)

target_link_libraries(aurora_rhi PUBLIC glad)
if (WIN32)
  target_link_libraries(aurora_rhi PUBLIC opengl32)
endif()


