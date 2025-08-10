add_library(aurora_platform STATIC
    src/Time.cpp
    src/WindowFactory.cpp
    src/Windows/Win32Window.cpp
)

target_include_directories(aurora_platform PUBLIC include)

target_link_libraries(aurora_platform PUBLIC aurora_core)

if (WIN32)
  target_link_libraries(aurora_platform PUBLIC user32)
endif()


