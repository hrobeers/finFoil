#-------------------------------------------------
#
# Project created by QtCreator 2013-03-01T20:14:20
#
#-------------------------------------------------

QT       += core gui

TARGET = unittest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../ \
                ../patheditor/ \
                ../fineditors/ \
                ../hrlib/

DEPENDPATH += ../

SOURCES += main.cpp

HEADERS +=

include(../hrlib/hrlib.pri)
include(../patheditor/patheditor.pri)

include(patheditor-tests/patheditor-tests.pri)
