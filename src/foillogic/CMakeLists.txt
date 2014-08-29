project(foillogic)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/foillogicfwd")

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

# hrlib
target_link_libraries(foillogic hrlib)
target_include_directories(foillogic PRIVATE "${CMAKE_SOURCE_DIR}/src/hrlib")

# patheditor
target_link_libraries(foillogic patheditor)
target_include_directories(foillogic PRIVATE "${CMAKE_SOURCE_DIR}/src/patheditor")

# JenSON
target_link_libraries(foillogic jenson)
target_include_directories(foillogic PRIVATE "${CMAKE_SOURCE_DIR}/submodules/jenson/src")

