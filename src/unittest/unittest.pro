#-------------------------------------------------
#
# Project created by QtCreator 2013-03-01T20:14:20
#
#-------------------------------------------------

QT       += core gui testlib
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# C++11 compiler flags depending on Qt version
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
equals(QT_MAJOR_VERSION, 4): QMAKE_CXXFLAGS += -std=c++0x

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

SOURCES += main.cpp

HEADERS +=

include(../hrlib/hrlib.pri)
include(../patheditor/patheditor.pri)

include(patheditor-tests/patheditor-tests.pri)
include(hrlib-tests/hrlib-tests.pri)
