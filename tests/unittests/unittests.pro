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

INCLUDEPATH +=  ../../src/ \
                ../../ \
                ../../src/patheditor/ \
                ../../src/fineditors/ \
                ../../src/hrlib/

DEPENDPATH += ../

SOURCES += main.cpp \
    patheditor-tests/quadrantrestrictortest.cpp \
    patheditor-tests/serializationtests.cpp

HEADERS += \
    patheditor-tests/quadrantrestrictortest.h \
    patheditor-tests/serializationtests.h

include(../../src/hrlib/hrlib.pri)
include(../../src/patheditor/patheditor.pri)
