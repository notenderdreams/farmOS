add_library(farmOS_cli STATIC
    cli.cpp
    color.cpp
)

target_include_directories(farmOS_cli
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)