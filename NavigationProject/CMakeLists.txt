cmake_minimum_required(VERSION 3.16)

project(NavigationProject LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(navigate
    src/main.cpp
    src/DataLoader.cpp
    src/Graph.cpp
    src/MapRenderer.cpp
)

target_include_directories(navigate PRIVATE include)
