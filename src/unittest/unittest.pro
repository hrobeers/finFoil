#-------------------------------------------------
#
# Project created by QtCreator 2013-03-01T20:14:20
#
#-------------------------------------------------

QT       += core gui testlib

include(../../common.pri)

TARGET = unittest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH +=  ../ \
                ../../ \
                ../patheditor/ \
                ../fineditors/ \
                ../hrlib/

DEPENDPATH += ../

SOURCES += main.cpp \
    patheditor-tests/quadrantrestrictortest.cpp

HEADERS += \
    patheditor-tests/quadrantrestrictortest.h

include(../hrlib/hrlib.pri)
include(../patheditor/patheditor.pri)
