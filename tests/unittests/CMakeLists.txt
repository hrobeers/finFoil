
set(TESTS_BINARY_NAME "finFoil-tests")

set(CMAKE_AUTOUIC ON)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

# Sources
file(GLOB_RECURSE SRC *.cpp)

# Headers
file(GLOB_RECURSE HDR *.hpp)


#
# The executable
#

add_executable(${TESTS_BINARY_NAME} WIN32 ${SRC} ${HDR} ${MOC_SRC} ${FORMS_HEADERS} ${RC_SRC} ${QM})
set_property(TARGET ${TESTS_BINARY_NAME} PROPERTY CXX_STANDARD 17)


#
# Linking
#

target_link_libraries(${TESTS_BINARY_NAME}
    Qt5::Core
    Qt5::Test
    hrlib
    finfoil_patheditor
    finfoil_logic
    jenson
)
