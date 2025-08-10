cmake_minimum_required(VERSION 3.20)

project(AuroraEngine LANGUAGES CXX)

# Allow subprojects with old policy requirements to configure under CMake >= 3.5
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(AURORA_BUILD_EDITOR "Build the editor application" OFF)
option(AURORA_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(MSVC)
  add_compile_options(/W4)
  if(AURORA_WARNINGS_AS_ERRORS)
    add_compile_options(/WX)
  endif()
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(AURORA_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
  endif()
endif()

add_subdirectory(engine)
add_subdirectory(apps/Runtime)


