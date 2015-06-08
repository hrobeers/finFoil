project(patheditor)

set(patheditorSRC
    controlpoint.cpp
    cubicbezier.cpp
    curvepoint.cpp
    editablepath.cpp
    line.cpp
    linerestrictor.cpp
    path.cpp
    pathdecorators.cpp
    patheditorview.cpp
    patheditorwidget.cpp
    pathitem.cpp
    pathpoint.cpp
    pathsettings.cpp
    pointhandle.cpp
    pointrestrictor.cpp
    quadrantrestrictor.cpp
    scalableimage.cpp
    scalepoint.cpp
)

set(patheditorHDR
    controlpoint.h
    cubicbezier.h
    curvepoint.hpp
    editablepath.h
    ipath.hpp
    line.h
    linerestrictor.h
    pathdecorators.hpp
    patheditorfwd/patheditorfwd.h
    patheditorview.h
    patheditorwidget.h
    pathfunctors.hpp
    path.h
    pathitem.h
    pathpoint.h
    pathsettings.h
    pathtemplates.hpp
    pointhandle.h
    pointrestrictor.h
    quadrantrestrictor.h
    restrictor.h
    scalableimage.h
    scalepoint.h
)

add_library(patheditor ${FINFOIL_LIB_TYPE} ${patheditorSRC} ${patheditorHDR})

target_link_libraries(patheditor
    Qt5::Widgets
    jenson
    hrlib
)
