cmake_minimum_required(VERSION 3.16)
project(GrowYourFarm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib CONFIG REQUIRED)

add_executable(GrowYourFarm main.cpp)
target_link_libraries(GrowYourFarm PRIVATE raylib)
