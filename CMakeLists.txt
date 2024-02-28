cmake_minimum_required(VERSION 3.1)
project(FluidSimulator)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Add your source files here
set(SOURCE_FILES
        main.cpp
)

# Add the executable target
add_executable(FluidSimulator ${SOURCE_FILES})

# Find SFML
find_package(SFML 2.6 COMPONENTS system window graphics audio REQUIRED)

# Link SFML libraries
target_link_libraries(FluidSimulator sfml-system sfml-window sfml-graphics sfml-audio)

# Find wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# Link wxWidgets libraries
target_link_libraries(FluidSimulator ${wxWidgets_LIBRARIES})

# (Optional) Set additional flags for wxWidgets
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")

# (Optional) Set include directories for wxWidgets
include_directories(${wxWidgets_INCLUDE_DIRS})
