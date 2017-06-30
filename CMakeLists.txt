cmake_minimum_required(VERSION 2.8.11)


#
# Project settings
#

project(finFoil)
set(PROJECT_BRIEF "${PROJECT_NAME} is a foil design tool optimized for surfboard fins.")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin/)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib/)

# Version
set(MAJOR_VERSION "1")
set(MINOR_VERSION "1")
set(REVISION "1")


#
# Build flags & compiler settings
#

# Define the Build flags
option(Shared "Build shared libararies where possible." OFF)
option(CCache "Build using ccache." OFF)
option(Tests "Build the tests executable" OFF)
option(Web "Include the web components" ON)

# Build flags
set(DEBUG_FLAGS "-g -O0 -Wall -Wextra -pedantic")
set(RELEASE_FLAGS "-O3 -fomit-frame-pointer -finline-functions -ffast-math")

# Assign the build flags
#set(CMAKE_VERBOSE_MAKEFILE OFF)
set(CMAKE_CXX_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_C_FLAGS_RELEASE ${RELEASE_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_CXX_FLAGS_DEBUG ${GCOV_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${DEBUG_FLAGS})
set(CMAKE_C_FLAGS_DEBUG ${GCOV_FLAGS})
set(CMAKE_EXE_LINKER_FLAGS_DEBUG ${GCOV_FLAGS})

# TODO OP check how this works on multiple configurations generators
# available configurations in multiple configurations enabled generators
set(CMAKE_CONFIGURATION_TYPES Debug Release)
#message("CMAKE_CFG_INTDIR = ${CMAKE_CFG_INTDIR}")

# Linking and dependency options
add_definitions(-DJENSON_QPTR) # Definition to make jenson serialize to qunique_ptr
if(Shared)
    set(FINFOIL_LIB_TYPE SHARED)
    set(JENSON_SHARED ON)
    set(QTUNITS_SHARED ON)
else()
    set(FINFOIL_LIB_TYPE STATIC)
    add_definitions(-DJENSON_STATIC)
    add_definitions(-DQTUNITS_STATIC)
endif()

# Optionally enable ccache
if(CCache)
    SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

# Optionally enable Web
if(Web)
else()
    add_definitions(-DWEB_DISABLED)
endif()


#
# Find and include external dependencies
#

# Qt
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Gui REQUIRED)
if(Web)
    find_package(Qt5Network REQUIRED)
    find_package(Qt5WebKitWidgets REQUIRED)
endif()
set(CMAKE_AUTOMOC ON)

# Boost
find_package(Boost REQUIRED)
if(Boost_FOUND)
  message(STATUS "Including Boost from: ${Boost_INCLUDE_DIRS}")
  include_directories(${Boost_INCLUDE_DIRS})
endif()


#
# Try to set build number and commit hash
#

# Get the build number
execute_process(
    COMMAND git rev-list HEAD --count
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_BUILD_NUMBER
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Get the commit hash
execute_process(
    COMMAND git rev-parse --verify HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

if(${GIT_BUILD_NUMBER})
    configure_file(
        ${CMAKE_SOURCE_DIR}/src/version_template.hpp
        ${CMAKE_SOURCE_DIR}/src/version_autogen.hpp
    )
    message(STATUS "GIT_BUILD_NUMBER: ${GIT_BUILD_NUMBER} GIT_COMMIT_HASH: ${GIT_COMMIT_HASH}")
else()
    message(WARNING "Not building from a git repository, the build number cannot be updated.")
endif()

#
# Include subdirectories
#

# Include header files
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/src/app")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/submodules/jenson/src")
INCLUDE_DIRECTORIES("${CMAKE_SOURCE_DIR}/submodules/qtunits/src")


add_subdirectory(src/app)
if(Tests)
    find_package(Qt5Test REQUIRED)
    add_subdirectory(tests/unittests)
endif()

# Install header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/.
  DESTINATION include/finfoil
  COMPONENT headers
)

# Install readme and license files
install(FILES
  ${PROJECT_SOURCE_DIR}/README
  ${PROJECT_SOURCE_DIR}/LICENSE.BSD
  ${PROJECT_SOURCE_DIR}/LICENSE.LGPLv2
  DESTINATION include/finfoil
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
)

# Note: executable and library installation is configured in dedicated CMakeLists.txt
