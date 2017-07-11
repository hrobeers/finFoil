set(LIB_NAME "finfoil_logic")
project(${LIB_NAME})

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/foillogic/*.hpp)

set(SRC
    foil.cpp
    foilcalculator.cpp
    contourcalculator.cpp
    profile.cpp
    profileloader.cpp
    thicknessprofile.cpp
    outline.cpp
)

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(${LIB_NAME}
    finfoil_patheditor
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
)
