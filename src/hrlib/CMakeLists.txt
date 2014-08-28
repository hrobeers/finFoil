project(hrlib)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/hrlibfwd")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/math")

set(hrlibSRC
    exceptions.cpp
    version.cpp
    math/spline.cpp
)

set(hrlibHDR
    exceptions.h
    version.h
    hrlibfwd/qtfwd.h
    math/spline.hpp
)

add_library(hrlib ${hrlibSRC} ${hrlibHDR})

target_link_libraries(hrlib Qt5::Core)
