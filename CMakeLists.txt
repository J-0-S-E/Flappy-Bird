# CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(Flappy_Bird)

# Legg til threepp som en ekstern mappe hvis den ligger lokalt
add_subdirectory(threepp)

# Legg til inkluderingsmapper
include_directories(threepp/include)

# Legg til hovedprogrammet
add_executable(Flappy_Bird main.cpp)

# Link threepp-biblioteket til ditt prosjekt
target_link_libraries(Flappy_Bird PRIVATE threepp)
