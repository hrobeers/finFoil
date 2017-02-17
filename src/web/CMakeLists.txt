project(web)

set(CMAKE_AUTOUIC ON)
qt5_wrap_ui(FORMS_HEADERS exportdialog.ui)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # the auto generated ui_*.h files

file(GLOB_RECURSE HDR ${CMAKE_SOURCE_DIR}/include/web/*.hpp)

set(SRC
    exportdialog.cpp
    stlexport.cpp
)

add_library(web ${FINFOIL_LIB_TYPE} ${SRC} ${HDR})

target_link_libraries(web
    Qt5::Core
    Qt5::Network
    Qt5::Widgets
    Qt5::WebKitWidgets
)
