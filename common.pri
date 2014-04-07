#
# This file contains common .pro file configuration
# (for app.pro and unittest.pro)
#

# Qt5 needs widgets explicitly configured
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# C++11 compiler flags depending on Qt version
greaterThan(QT_MAJOR_VERSION, 4): CONFIG += c++11
equals(QT_MAJOR_VERSION, 4): QMAKE_CXXFLAGS += -std=c++0x

# Disable reorder warning
QMAKE_CXXFLAGS += -Wno-reorder

# JenSON lib include
unix|win32: LIBS += -L$${_PRO_FILE_PWD_}/../../submodules/jenson/bin/ -ljenson
INCLUDEPATH += $${_PRO_FILE_PWD_}/../../submodules/jenson/src/
