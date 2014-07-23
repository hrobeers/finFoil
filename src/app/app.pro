#-------------------------------------------------
#
# Project created by QtCreator 2012-12-08T00:32:05
#
#-------------------------------------------------

QT       += core gui

include(../../common.pri)

TARGET = finFoil
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    finfoil_version.h \
    ../version_autogen.h \
    runinteractive.hpp \
    main.h \
    about.hpp

FORMS    += mainwindow.ui

INCLUDEPATH +=  ../ \
                ../patheditor/ \
                ../foileditors/ \
                ../hrlib/ \
                ../foillogic/

DEPENDPATH += ..

include(../patheditor/patheditor.pri)
include(../foileditors/foileditors.pri)
include(../hrlib/hrlib.pri)
include(../foillogic/foillogic.pri)

OTHER_FILES +=  ../../LICENSE.BSD \
                ../../LICENSE.LGPLv2 \
                ../../README \
                ../../TODO

#
# QtUnits lib include
#

unix|win32: LIBS += -L$${_PRO_FILE_PWD_}/../../submodules/qtunits/bin/ -lQtUnits

INCLUDEPATH += $${_PRO_FILE_PWD_}/../../submodules/qtunits/src/
