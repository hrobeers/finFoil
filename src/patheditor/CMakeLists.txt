project(patheditor)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/patheditorfwd")

set(patheditorSRC
    editablepath.cpp
    pathsettings.cpp
    pathitem.cpp
    cubicbezier.cpp
    patheditorwidget.cpp
    pointhandle.cpp
    line.cpp
    linerestrictor.cpp
    pointrestrictor.cpp
    pathpoint.cpp
    quadrantrestrictor.cpp
    controlpoint.cpp
    patheditorview.cpp
    path.cpp
    scalepoint.cpp
    scalableimage.cpp
)

set(patheditorHDR
    editablepath.h
    pathsettings.h
    pathitem.h
    cubicbezier.h
    patheditorwidget.h
    pointhandle.h
    line.h
    restrictor.h
    linerestrictor.h
    pointrestrictor.h
    pathpoint.h
    patheditorfwd/patheditorfwd.h
    quadrantrestrictor.h
    controlpoint.h
    patheditorview.h
    path.h
    scalepoint.h
    scalableimage.h
    pathtemplates.hpp
    pathfunctors.hpp
)

add_library(patheditor ${patheditorSRC} ${patheditorHDR})

# hrlib
add_subdirectory(../hrlib "${CMAKE_SOURCE_DIR}/src/hrlib")
target_link_libraries(patheditor hrlib)
target_include_directories(patheditor PRIVATE "${CMAKE_SOURCE_DIR}/src/hrlib")

target_link_libraries(patheditor Qt5::Widgets)

