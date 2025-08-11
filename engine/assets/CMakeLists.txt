add_library(aurora_assets STATIC
    src/AssetManager.cpp
)

target_include_directories(aurora_assets PUBLIC include)

target_link_libraries(aurora_assets PUBLIC aurora_core aurora_rhi)


