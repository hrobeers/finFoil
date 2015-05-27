project(foileditors)

set(foileditorsSRC
    profileeditor/profileeditor.cpp
    thicknesseditor/thicknesseditor.cpp
    outlineeditor/thicknesscontours.cpp
    outlineeditor/outlineeditor.cpp
    foildatawidget.cpp
)

set(foileditorsHDR
    profileeditor/profileeditor.h
    thicknesseditor/thicknesseditor.h
    foileditors.h
    outlineeditor/thicknesscontours.h
    outlineeditor/outlineeditor.h
    foildatawidget.h
    foileditorsfwd/foileditorsfwd.h
)

add_library(foileditors ${FINFOIL_LIB_TYPE} ${foileditorsSRC} ${foileditorsHDR})

target_link_libraries(foileditors
    qtunits
    foillogic
)
