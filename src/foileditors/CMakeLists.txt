set(LIB_NAME "finfoil_editors")
project(${LIB_NAME})

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/foileditors/*.hpp)

set(SRC
    profileeditor/profileeditor.cpp
    thicknesseditor/thicknesseditor.cpp
    outlineeditor/thicknesscontours.cpp
    outlineeditor/outlineeditor.cpp
    foildatawidget.cpp
)

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(${LIB_NAME}
    qtunits
    finfoil_logic
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)
