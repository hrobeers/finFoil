#-------------------------------------------------
#
# Project created by QtCreator 2012-12-08T00:32:05
#
#-------------------------------------------------

QT       += core gui

TARGET = finFoil
TEMPLATE = app

CONFIG(proceduralDebug) {
    DEFINES += PROCEDURAL
}

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
    finfoil_version.h \
    ../version_autogen.h

FORMS    += mainwindow.ui

INCLUDEPATH +=  ../ \
                ../patheditor/ \
                ../fineditors/ \
                ../hrlib/

DEPENDPATH += ..

include(../patheditor/patheditor.pri)
include(../fineditors/fineditors.pri)
include(../hrlib/hrlib.pri)

OTHER_FILES +=  ../../LICENSE.BSD \
                ../../LICENSE.LGPLv2 \
                ../../README \
                ../../TODO
