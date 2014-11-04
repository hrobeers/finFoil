project(hrlib)

set(hrlibSRC
    exceptions.cpp
    math/spline.cpp
    version.cpp
)

set(hrlibHDR
    exceptions.h
    hrlibfwd/qtfwd.h
    math/spline.hpp
    mixin/historical.hpp
    mixin/identifiable.hpp
    patterns/decorator.hpp
    version.h
)

add_library(hrlib ${hrlibSRC} ${hrlibHDR})

target_link_libraries(hrlib Qt5::Core)
