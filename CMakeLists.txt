cmake_minimum_required(VERSION 2.8.11)

project(finFoil)

set(BINARY_NAME "finFoil")

# Define the Build options
option(DebugBuild "Build debug binaries." OFF)
option(CCache "Build using ccache." OFF)

# Set the CMAKE_BUILD_TYPE (Only DEBUG and RELEASE are supported)
if(DebugBuild)
    set(CMAKE_BUILD_TYPE DEBUG)
    message(STATUS "CMAKE_BUILD_TYPE = DEBUG")
else()
    set(CMAKE_BUILD_TYPE RELEASE)
    message(STATUS "CMAKE_BUILD_TYPE = RELEASE")
endif()

# Optionally set the compiler
if(CCache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

# Set the compilation flags
set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall -Wextra -pedantic -Wno-reorder")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fomit-frame-pointer -finline-functions -ffast-math")

# Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
set(CMAKE_AUTOMOC ON)

# Include the subdirectories
add_subdirectory(src/app)
