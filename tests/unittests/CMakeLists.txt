set(CMAKE_AUTOUIC ON)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

# Sources
set(SRC
    main.cpp
    hrlib-tests/decoratortests.cpp
    patheditor-tests/quadrantrestrictortest.cpp
    patheditor-tests/serializationtests.cpp
)

# Headers
set(HDR
    ../../submodules/qtestrunner/qtestrunner.hpp
    hrlib-tests/decoratortests.hpp
    patheditor-tests/quadrantrestrictortest.h
    patheditor-tests/serializationtests.h
)


#
# The executable
#

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
add_executable(${TESTS_BINARY_NAME} WIN32 ${SRC} ${HDR} ${MOC_SRC} ${FORMS_HEADERS} ${RC_SRC} ${QM})


#
# Linking
#

#Qt modules
target_link_libraries(${TESTS_BINARY_NAME}
    Qt5::Core
    Qt5::Test
    hrlib
    patheditor
    foillogic
    jenson
)
