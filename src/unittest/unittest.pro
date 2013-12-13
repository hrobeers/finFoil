#-------------------------------------------------
#
# Project created by QtCreator 2013-03-01T20:14:20
#
#-------------------------------------------------

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = unittest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../ \
                ../patheditor/ \
                ../fineditors/ \
                ../hrlib/ \
                ../hrtestlib \

DEPENDPATH += ../

QMAKE_CXXFLAGS +=   -std=c++11 \
                    -Wno-multichar \
                    -Wno-reorder

SOURCES += main.cpp

HEADERS +=

include(../hrlib/hrlib.pri)
include(../hrtestlib/hrtestlib.pri)
include(../patheditor/patheditor.pri)

include(patheditor-tests/patheditor-tests.pri)
include(hrlib-tests/hrlib-tests.pri)
