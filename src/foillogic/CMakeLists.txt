project(foillogic)

set(foillogicSRC
    foil.cpp
    foilcalculator.cpp
    contourcalculator.cpp
    profile.cpp
    thicknessprofile.cpp
    outline.cpp
)

set(foillogicHDR
    foil.h
    foillogicfwd/foillogicfwd.h
    foilcalculator.h
    contourcalculator.h
    profile.h
    thicknessprofile.h
    outline.h
)

add_library(foillogic ${foillogicSRC} ${foillogicHDR})

target_link_libraries(foillogic patheditor)
