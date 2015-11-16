project(hrlib)

set(hrlibSRC
    exceptions.cpp
    math/brent.cpp
    math/spline.cpp
    string/json_utils.cpp
    version.cpp
)

set(hrlibHDR
    exceptions.h
    hrlibfwd/qtfwd.h
    math/brent.hpp
    math/spline.hpp
    mixin/historical.hpp
    mixin/identifiable.hpp
    patterns/decorator.hpp
    string/json_utils.hpp
    version.h
)

add_library(hrlib ${FINFOIL_LIB_TYPE} ${hrlibSRC} ${hrlibHDR})

target_link_libraries(hrlib Qt5::Core)
