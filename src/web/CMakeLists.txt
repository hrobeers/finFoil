set(LIB_NAME "finfoil_web")
project(${LIB_NAME})

set(CMAKE_AUTOUIC ON)
qt5_wrap_ui(FORMS_HEADERS_WEB exportdialog.ui)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # the auto generated ui_*.h files

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/web/*.hpp)

set(SRC
    exportdialog.cpp
    stlexport.cpp
)

add_library(${LIB_NAME} ${FINFOIL_LIB_TYPE} ${SRC} ${HDR} ${FORMS_HEADERS_WEB})

target_link_libraries(${LIB_NAME}
    Qt5::Core
    Qt5::Network
    Qt5::Widgets
    Qt5::WebEngineWidgets
)

install(TARGETS ${LIB_NAME}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
)
