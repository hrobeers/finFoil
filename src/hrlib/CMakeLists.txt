project(hrlib)

include_directories(${CMAKE_SOURCE_DIR}/include/hrlib/hrlibfwd)
include_directories(${CMAKE_SOURCE_DIR}/include/hrlib/math)
include_directories(${CMAKE_SOURCE_DIR}/include/hrlib/mixin)
include_directories(${CMAKE_SOURCE_DIR}/include/hrlib/string)

set(hrlibSRC
    exceptions.cpp
    math/brent.cpp
    math/spline.cpp
    string/json_utils.cpp
    version.cpp
)

add_library(hrlib ${FINFOIL_LIB_TYPE} ${hrlibSRC} ${hrlibHDR})

target_link_libraries(hrlib Qt5::Core)
