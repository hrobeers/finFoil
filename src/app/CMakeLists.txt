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

# prepare (generated) icon support for .app bundle

if (APPLE)
  set(MACOSX_BUNDLE_ICON_FILE ${BINARY_NAME}.icns)
  set(ICNS ${CMAKE_BINARY_DIR}/${BINARY_NAME}.icns)
  # TODO: this seems to copy the icns file to the .app bundle AFTER the DMG
  # is constructed. for now, the copying is done by the DMG creation
  set_source_files_properties(${ICNS} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

  # custom command to generate iconset when its source is needed
  add_custom_command(
    OUTPUT ${ICNS}
    COMMAND ../scripts/gen-icns.sh ../src/resources/${BINARY_NAME}.svg
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )
endif()

#
# The executable
#

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
if (APPLE)
  add_executable(${BINARY_NAME} WIN32 MACOSX_BUNDLE ${ICNS} ${SRC} ${HDR} ${MOC_SRC} ${FORMS_HEADERS} ${RC_SRC} ${QM})
else (APPLE)
  add_executable(${BINARY_NAME} WIN32 ${SRC} ${HDR} ${MOC_SRC} ${FORMS_HEADERS} ${RC_SRC} ${QM})
endif (APPLE)

#
# Subdirectories
#

add_subdirectory(../hrlib "${CMAKE_SOURCE_DIR}/src/hrlib")
add_subdirectory(../patheditor "${CMAKE_SOURCE_DIR}/src/patheditor")
add_subdirectory(../foillogic "${CMAKE_SOURCE_DIR}/src/foillogic")
add_subdirectory(../foileditors "${CMAKE_SOURCE_DIR}/src/foileditors")
add_subdirectory(../../submodules/jenson/src "${CMAKE_SOURCE_DIR}/submodules/jenson/src")
add_subdirectory(../../submodules/qtunits/src "${CMAKE_SOURCE_DIR}/submodules/qtunits/src")
if (Web)
    add_subdirectory(../web "${CMAKE_SOURCE_DIR}/src/web")
endif()


#
# Linking
#

target_link_libraries(${BINARY_NAME}
    foileditors
)
if (Web)
target_link_libraries(${BINARY_NAME} web)
endif()


# add enough libraries to the app bundle to allow stand-alone execution
if (APPLE)
  find_program(MACDEPLOYQT NAMES macdeployqt PATHS ${qt_base_dir}/bin)
  if(NOT MACDEPLOYQT)
      message(FATAL_ERROR "The utility 'macdeployqt' is required to create an App Bundle!")
  endif()

  # populate the app bundle with Qt(5) support using macdeployqt
  add_custom_command(
    TARGET ${BINARY_NAME}
    POST_BUILD
    COMMAND ${MACDEPLOYQT} ${BINARY_NAME}.app
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  # find out the location of qt5 plugins
  # current paths include: location via MacPort installation
  # TODO: add more possible paths + allow for envrionment variable to override
  set(plugins_search_path
    /opt/local/share/qt5/  # MacPorts
    /usr/local/share/qt5/  # Homebrew (unverified)
  )
  find_path(QT5SHARE NAMES plugins PATHS ${plugins_search_path})
  if(NOT QT5SHARE)
      message(FATAL_ERROR "Could not determine qt5/plugins base directory.")
  endif()
  set(QT5PLUGINS "${QT5SHARE}/plugins")

  # find out the location of the Frameworks
  # current paths include: location via MacPort installation
  # TODO: add more possible paths + allow for envrionment variable to override
  set(frameworks_search_path
    /opt/local/Library/Frameworks           # MacPorts
    /usr/local/Library/Frameworks           # Homebrew (unverified)
  )
  find_library(QTPRINTSUPPORT_FW NAMES QtPrintSupport PATHS ${frameworks_search_path})
  if(NOT QTPRINTSUPPORT_FW)
      message(FATAL_ERROR "QtPrintSupport.framework is required to create an App Bundle!")
  endif()
  get_filename_component(FRAMEWORKS ${QTPRINTSUPPORT_FW} DIRECTORY)

  # add dynamically loaded plugins, missing from the app bundle after prev cmd
  add_custom_command(
    TARGET ${BINARY_NAME}
    POST_BUILD
    COMMAND ../../scripts/fix-plugins.sh ${QT5PLUGINS} ${FRAMEWORKS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/${BINARY_NAME}.app
  )

  # create DMG
  add_custom_command(
    TARGET ${BINARY_NAME}
    POST_BUILD
    COMMAND ../scripts/create-dmg.sh ${BINARY_NAME}.app ${MAJOR_VERSION}.${MINOR_VERSION}.${REVISION}-${GIT_BUILD_NUMBER} ${ICNS}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  )

  # export compile-time variable for conditional compilation in main.cpp of code
  # to fix the library path, restricting it to the app bundle
  add_definitions(-DEXECUTE_WITHIN_APP_BUNDLE)
endif()
