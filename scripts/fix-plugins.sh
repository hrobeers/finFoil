#!/bin/bash

# script expects to be execute from the bundle directory (e.g. finFoil.app/)
# it takes two arguments:
LIBQCOCOA_DYLIB=$1
FRAMEWORKS_PATH=$2

# install libqcocoa plugin
mkdir -p Contents/PlugIns/platforms
cp ${LIBQCOCOA_DYLIB} Contents/PlugIns/platforms

# fix identity and references to frameworks
install_name_tool -id @executable_path/../PlugIns/platforms/libqcocoa.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtPrintSupport.framework/Versions/5/QtPrintSupport @executable_path/../Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore Contents/PlugIns/platforms/libqcocoa.dylib

# fix for issue #28, macports seems to override the system version
# TODO: check other situations, e.g. without MacPorts, with Homebrew,...
install_name_tool -change /opt/local/lib/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change /opt/local/lib/libgthread-2.0.0.dylib @executable_path/../Frameworks/libgthread-2.0.0.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change /opt/local/lib/libglib-2.0.0.dylib @executable_path/../Frameworks/libglib-2.0.0.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change /opt/local/lib/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib Contents/PlugIns/platforms/libqcocoa.dylib

# install QtPrintSupport framework
cp -r ${FRAMEWORKS_PATH}/QtPrintSupport.framework Contents/Frameworks

# fix identity and references to frameworks
install_name_tool -id @executable_path/../Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
install_name_tool -change ${FRAMEWORKS_PATH}/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
install_name_tool -change ${FRAMEWORKS_PATH}/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
install_name_tool -change ${FRAMEWORKS_PATH}/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
