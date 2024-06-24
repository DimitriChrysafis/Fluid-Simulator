cmake_minimum_required(VERSION 3.28)
project(Fourier)

set(CMAKE_CXX_STANDARD 17)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics audio REQUIRED)

# Add the executable
add_executable(Fourier main.cpp
        BallSimulation.hpp
        BallSimulation.cpp)

# Link SFML libraries to your target
target_link_libraries(Fourier sfml-graphics sfml-audio)
