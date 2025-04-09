cmake_minimum_required(VERSION 3.20)

# Project name and version
project(HTTPServer VERSION 1.0 LANGUAGES CXX)
# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(include)

# Find all source files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Create the executable from all source files
add_executable(${PROJECT_NAME} ${SOURCES})
