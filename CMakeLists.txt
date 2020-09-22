cmake_minimum_required(VERSION 3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.18)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
else()
    cmake_policy(VERSION 3.18)
endif()

#set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE string "clang compiler" FORCE)

project(mfeed VERSION 1.0
    DESCRIPTION "Mart's RSS Feed Reader"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CURL_LIBRARY "-lcurl")
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

include(FetchContent)
FetchContent_Declare(
    fmtlib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 7.0.3
)
FetchContent_MakeAvailable(fmtlib)

add_subdirectory(src)

