project(foileditors)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/profileeditor")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/thicknesseditor")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/outlineeditor")

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

add_library(foileditors ${foileditorsSRC} ${foileditorsHDR})

target_link_libraries(foileditors Qt5::Core)

# hrlib
target_link_libraries(foileditors hrlib)
target_include_directories(foileditors PRIVATE "${CMAKE_SOURCE_DIR}/src/hrlib")

# patheditor
target_link_libraries(foileditors patheditor)
target_include_directories(foileditors PRIVATE "${CMAKE_SOURCE_DIR}/src/patheditor")

# foillogic
target_link_libraries(foileditors foillogic)
target_include_directories(foileditors PRIVATE "${CMAKE_SOURCE_DIR}/src/foillogic")

# JenSON
target_link_libraries(foileditors jenson)
target_include_directories(foileditors PRIVATE "${CMAKE_SOURCE_DIR}/submodules/jenson/src")

# QtUnits
target_link_libraries(foileditors qtunits)
target_include_directories(foileditors PRIVATE "${CMAKE_SOURCE_DIR}/submodules/qtunits/src")
