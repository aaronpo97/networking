cmake_minimum_required(VERSION 3.30.5)

project(HTTPServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(UNIX) # ONLY SUPPORT BUILDS ON UNIX-LIKE SYSTEMS
    file(GLOB_RECURSE SOURCES "src/*.cpp")

    include_directories(${CMAKE_SOURCE_DIR}/includes)

    include(FetchContent)
    FetchContent_Declare(
            json
            GIT_REPOSITORY https://github.com/nlohmann/json.git
            GIT_TAG v3.12.0
    )
    FetchContent_MakeAvailable(json)

    FetchContent_Declare(
            parser
            GIT_REPOSITORY https://github.com/aaronpo97/httpparser.git
    )
    FetchContent_MakeAvailable(parser)

    add_executable(${PROJECT_NAME} ${SOURCES})

    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            nlohmann_json::nlohmann_json
            httpparser
    )
else()
    message(FATAL_ERROR "This project only supports Unix-like systems.")
endif()
