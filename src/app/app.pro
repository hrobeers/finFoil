#-------------------------------------------------
#
# Project created by QtCreator 2012-12-08T00:32:05
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = finFoil
TEMPLATE = app

QMAKE_CXXFLAGS +=   -std=c++11 \
                    -Wno-multichar \
                    -Wno-reorder

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    finfoil_version.h \
    ../version_autogen.h

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

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../QtUnits/src/QtUnits/release/ -lQtUnits
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../QtUnits/src/QtUnits/debug/ -lQtUnits
else:unix: LIBS += -L$$PWD/../../../QtUnits/src/QtUnits/ -lQtUnits

INCLUDEPATH += $$PWD/../../../QtUnits/src/QtUnits
DEPENDPATH += $$PWD/../../../QtUnits/src/QtUnits
