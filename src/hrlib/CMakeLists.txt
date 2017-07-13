set(LIB_NAME "hrlib")
project(${LIB_NAME})

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.hpp)

set(SRC
    exceptions.cpp
    math/brent.cpp
    math/spline.cpp
    string/json_utils.cpp
    version.cpp
)

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(${LIB_NAME} Qt5::Core)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
)
