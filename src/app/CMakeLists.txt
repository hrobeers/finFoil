set(CMAKE_AUTOUIC ON)
qt5_wrap_ui(FORMS_HEADERS mainwindow.ui)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}) # the auto generated ui_*.h files

# Sources
set(SRC
    main.cpp
    mainwindow.cpp
)

# Headers
set(HDR
    mainwindow.h
    finfoil_version.h
    ../version_autogen.h
    ../version_template.h
    runinteractive.hpp
    main.h
    about.hpp
)


#
# The executable
#

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${BINARY_NAME} WIN32 MACOSX_BUNDLE ${SRC} ${HDR} ${MOC_SRC} ${FORMS_HEADERS} ${RC_SRC} ${QM})

#
# Subdirectories
#

add_subdirectory(../hrlib "${CMAKE_SOURCE_DIR}/src/hrlib")
add_subdirectory(../patheditor "${CMAKE_SOURCE_DIR}/src/patheditor")
add_subdirectory(../foillogic "${CMAKE_SOURCE_DIR}/src/foillogic")
add_subdirectory(../foileditors "${CMAKE_SOURCE_DIR}/src/foileditors")
add_subdirectory(../../submodules/jenson/src "${CMAKE_SOURCE_DIR}/submodules/jenson/src")
add_subdirectory(../../submodules/qtunits/src "${CMAKE_SOURCE_DIR}/submodules/qtunits/src")


#
# Linking
#

target_link_libraries(${BINARY_NAME} foileditors)
