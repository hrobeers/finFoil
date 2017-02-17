project(hrlib)

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/hrlib/*.hpp)

set(SRC
    exceptions.cpp
    math/brent.cpp
    math/spline.cpp
    string/json_utils.cpp
    version.cpp
)

add_library(hrlib ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(hrlib Qt5::Core)
