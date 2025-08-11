add_library(aurora_rhi STATIC
    src/RHI.cpp
    src/Null/NullDevice.cpp
    src/OpenGL/GLDevice.cpp
    src/OpenGL/GLRenderPass.hpp
    src/OpenGL/GLSwapchain.hpp
    src/OpenGL/GLShaderModule.cpp
    src/OpenGL/GLShaderModule.hpp
    src/OpenGL/GLBuffer.cpp
    src/OpenGL/GLBuffer.hpp
    src/OpenGL/GLGraphicsPipeline.cpp
    src/OpenGL/GLGraphicsPipeline.hpp
    src/OpenGL/GLDescriptorSet.hpp
    src/OpenGL/GLTexture.cpp
    src/OpenGL/GLTexture.hpp
    src/OpenGL/GLSampler.cpp
    src/OpenGL/GLSampler.hpp
    src/OpenGL/GLState.cpp
    src/OpenGL/GLState.hpp
    src/OpenGL/GLConversions.cpp
    src/OpenGL/GLConversions.hpp
    src/OpenGL/GLCapabilities.cpp
    src/OpenGL/GLCapabilities.hpp
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


