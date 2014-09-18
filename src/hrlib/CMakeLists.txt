project(hrlib)

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
    mixin/identifiable.hpp
)

add_library(hrlib ${hrlibSRC} ${hrlibHDR})

target_link_libraries(hrlib Qt5::Core)
