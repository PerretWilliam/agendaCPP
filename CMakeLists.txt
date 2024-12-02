cmake_minimum_required(VERSION 3.10)

# Set the project name
project(Agenda)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add include directories
include_directories(include)

# Add source files
file(GLOB SOURCES "src/*.cpp")

# Add the executable
add_executable(agenda ${SOURCES})