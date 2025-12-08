add_library(farmOS_core STATIC
    farm_os.cpp
)

# Expose the "src" directory as a public include directory so headers can be
# included as "core/farm_os.h" from anywhere in the project.
target_include_directories(farmOS_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)
