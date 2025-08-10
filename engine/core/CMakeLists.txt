add_library(aurora_core STATIC
    src/Log.cpp
)

target_include_directories(aurora_core PUBLIC include)

target_compile_definitions(aurora_core
    PUBLIC
      $<$<CONFIG:Debug>:AURORA_DEBUG>
)


