#-------------------------------------------------
#
# Project created by QtCreator 2012-12-08T00:32:05
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# C++11 compiler flags depending on Qt version
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
equals(QT_MAJOR_VERSION, 4): QMAKE_CXXFLAGS += -std=c++0x

TARGET = finFoil
TEMPLATE = app

QMAKE_CXXFLAGS +=   -Wno-multichar \
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

unix|win32: LIBS += -L$${_PRO_FILE_PWD_}/../../../QtUnits/bin/ -lQtUnits

INCLUDEPATH += $${_PRO_FILE_PWD_}/../../../QtUnits/src/QtUnits
DEPENDPATH += $${_PRO_FILE_PWD_}/../../../QtUnits/src/QtUnits
