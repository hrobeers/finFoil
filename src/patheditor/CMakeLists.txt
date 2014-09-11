project(patheditor)

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

target_link_libraries(patheditor
    Qt5::Widgets
    jenson
    hrlib
)
