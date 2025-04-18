cmake_minimum_required(VERSION 3.20)

# Project name and version
project(HTTPServer VERSION 1.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory
include_directories(${CMAKE_SOURCE_DIR}/includes)

# Find all source files in the src directory
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Add nlohmann_json library via FetchContent
include(FetchContent)
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(json)

# Create the executable from all source files
add_executable(${PROJECT_NAME} ${SOURCES})

# Link nlohmann_json to the project
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
