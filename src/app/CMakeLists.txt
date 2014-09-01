set(CMAKE_AUTOUIC ON)

qt5_wrap_ui(FORMS_HEADERS mainwindow.ui)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

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
add_executable(${BINARY_NAME} WIN32 ${SRC} ${HDR} ${MOC_SRC} ${FORMS_HEADERS} ${RC_SRC} ${QM})


#
# Linking
#

#Qt modules
target_link_libraries(finFoil Qt5::Core Qt5::Widgets)

add_subdirectory(../hrlib "${CMAKE_SOURCE_DIR}/src/hrlib")
add_subdirectory(../patheditor "${CMAKE_SOURCE_DIR}/src/patheditor")
add_subdirectory(../foillogic "${CMAKE_SOURCE_DIR}/src/foillogic")
add_subdirectory(../foileditors "${CMAKE_SOURCE_DIR}/src/foileditors")
add_subdirectory(../../submodules/jenson "${CMAKE_SOURCE_DIR}/submodules/jenson")
add_subdirectory(../../submodules/qtunits "${CMAKE_SOURCE_DIR}/submodules/qtunits")

# hrlib
target_link_libraries(finFoil hrlib)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/src/hrlib")

# patheditor
target_link_libraries(finFoil patheditor)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/src/patheditor")

# foillogic
target_link_libraries(finFoil foillogic)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/src/foillogic")

#foileditors
target_link_libraries(finFoil foileditors)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/src/foileditors")

# JenSON
target_link_libraries(finFoil jenson)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/submodules/jenson/src")

# QtUnits
target_link_libraries(finFoil qtunits)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/submodules/qtunits/src")
