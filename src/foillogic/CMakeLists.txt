set(LIB_NAME "finfoil_logic")
project(${LIB_NAME})

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/foillogic/*.hpp)

set(SRC
    foil.cpp
    foilcalculator.cpp
    foilio.cpp
    profile.cpp
    samplers.cpp
    thicknessprofile.cpp
    outline.cpp
)

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})
set_property(TARGET ${LIB_NAME} PROPERTY CXX_STANDARD 17)

target_link_libraries(${LIB_NAME}
    finfoil_patheditor
    ${Boost_LIBRARIES}
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)
