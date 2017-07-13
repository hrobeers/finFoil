set(LIB_NAME "finfoil_patheditor")
project(${LIB_NAME})

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

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(${LIB_NAME}
    Qt5::Widgets
    jenson
    hrlib
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
)
