project(foileditors)

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/foileditors/*.hpp)

set(SRC
    profileeditor/profileeditor.cpp
    thicknesseditor/thicknesseditor.cpp
    outlineeditor/thicknesscontours.cpp
    outlineeditor/outlineeditor.cpp
    foildatawidget.cpp
)

add_library(foileditors ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(foileditors
    qtunits
    foillogic
)
