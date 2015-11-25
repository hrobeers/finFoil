project(web)

set(CMAKE_AUTOUIC ON)
qt5_wrap_ui(FORMS_HEADERS exportdialog.ui)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # the auto generated ui_*.h files

set(webSRC
    exportdialog.cpp
    stlexport.cpp
)

set(webHDR
    exportdialog.hpp
    stlexport.hpp
)

add_library(web ${FINFOIL_LIB_TYPE} ${webSRC} ${webHDR})

target_link_libraries(web
    Qt5::Core
    Qt5::Network
    Qt5::Widgets
    Qt5::WebKitWidgets
)
