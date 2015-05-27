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
    pathdecorators.cpp
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
    ipath.hpp
    path.h
    pathdecorators.hpp
    scalepoint.h
    scalableimage.h
    pathtemplates.hpp
    pathfunctors.hpp
)

add_library(patheditor ${FINFOIL_LIB_TYPE} ${patheditorSRC} ${patheditorHDR})

target_link_libraries(patheditor
    Qt5::Widgets
    jenson
    hrlib
)
