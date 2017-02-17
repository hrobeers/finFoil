project(foillogic)

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/foillogic/*.hpp)

set(SRC
    foil.cpp
    foilcalculator.cpp
    contourcalculator.cpp
    profile.cpp
    thicknessprofile.cpp
    outline.cpp
)

add_library(foillogic ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(foillogic
    patheditor
)
