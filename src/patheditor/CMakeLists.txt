project(patheditor)

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/patheditor/*.hpp)

set(SRC
    continuitylever.cpp
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
    pointcontextmenu.cpp
    pointhandle.cpp
    pointrestrictor.cpp
    quadrantrestrictor.cpp
    scalableimage.cpp
    scalepoint.cpp
)

add_library(patheditor ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(patheditor
    Qt5::Widgets
    jenson
    hrlib
)
