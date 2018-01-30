set(LIB_NAME "hrlib")
project(${LIB_NAME})

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/${LIB_NAME}/*.hpp)

file(GLOB_RECURSE SRC *.cpp)
file(GLOB_RECURSE SRC_C *.c *.h)

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${SRC_C} ${HDR})

target_link_libraries(${LIB_NAME} Qt5::Core)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)
