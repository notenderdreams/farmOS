cmake_minimum_required(VERSION 3.15)

project(farmOS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_subdirectory(src/core)
add_subdirectory(src/cli)
add_subdirectory(tests)
