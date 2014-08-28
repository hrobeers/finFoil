set(CMAKE_AUTOUIC ON)

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
    runinteractive.hpp
    main.h
    about.hpp
)


#
# The executable
#

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${BINARY_NAME} WIN32 ${SRC} ${HDR} ${MOC_SRC} ${RC_SRC} ${QM})


#
# Linking
#

#Qt modules
target_link_libraries(finFoil Qt5::Core Qt5::Widgets)

# foillogic
add_subdirectory(../foillogic "${CMAKE_SOURCE_DIR}/src/foillogic")
target_link_libraries(finFoil foillogic)
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/src/foillogic")

#foileditors
target_include_directories(finFoil PRIVATE "${CMAKE_SOURCE_DIR}/src/foileditors")
