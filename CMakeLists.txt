cmake_minimum_required(VERSION 3.29)
project(Flappy_Bird)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(threepp)

include_directories(threepp/include)

add_executable(Flappy_Bird main.cpp)

target_link_libraries(Flappy_Bird threepp)
