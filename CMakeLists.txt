cmake_minimum_required(VERSION 3.16)
project(sensor_system LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(sensor_system
    src/main.cpp
    src/model.cpp
    src/scenario_loader.cpp
)
