#!/bin/bash

# script expects to be executed from the bundle directory (e.g. finFoil.app/)
# it takes two arguments:
PLUGINS_PATH=$1
FRAMEWORKS_PATH=$2

LIB_DIR=/opt/local/lib # TODO: make this more generic

# install libqcocoa plugin
mkdir -p Contents/PlugIns/platforms
cp ${PLUGINS_PATH}/platforms/libqcocoa.dylib Contents/PlugIns/platforms/

# fix identity and references to frameworks
install_name_tool -id @executable_path/../PlugIns/platforms/libqcocoa.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtPrintSupport.framework/Versions/5/QtPrintSupport @executable_path/../Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${FRAMEWORKS_PATH}/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore Contents/PlugIns/platforms/libqcocoa.dylib

# fix for issue #28, macports seems to override the system version
# TODO: check other situations, e.g. without MacPorts, with Homebrew,...
install_name_tool -change ${LIB_DIR}/libz.1.dylib @executable_path/../Frameworks/libz.1.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${LIB_DIR}/libgthread-2.0.0.dylib @executable_path/../Frameworks/libgthread-2.0.0.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${LIB_DIR}/libglib-2.0.0.dylib @executable_path/../Frameworks/libglib-2.0.0.dylib Contents/PlugIns/platforms/libqcocoa.dylib
install_name_tool -change ${LIB_DIR}/libintl.8.dylib @executable_path/../Frameworks/libintl.8.dylib Contents/PlugIns/platforms/libqcocoa.dylib

# install QtPrintSupport framework
cp -r ${FRAMEWORKS_PATH}/QtPrintSupport.framework Contents/Frameworks

# fix identity and references to frameworks
install_name_tool -id @executable_path/../Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
install_name_tool -change ${FRAMEWORKS_PATH}/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
install_name_tool -change ${FRAMEWORKS_PATH}/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport
install_name_tool -change ${FRAMEWORKS_PATH}/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore Contents/Frameworks/QtPrintSupport.framework/Versions/5/QtPrintSupport

# fix for issue #28, missing imageformats
cp -r ${PLUGINS_PATH}/imageformats Contents/PlugIns/

# fix references to frameworks
for plugin in Contents/PlugIns/imageformats/*.dylib; do
  install_name_tool -id @executable_path/../../${plugin} ${plugin}
  install_name_tool -change ${FRAMEWORKS_PATH}/QtGui.framework/Versions/5/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/5/QtGui ${plugin}
  install_name_tool -change ${FRAMEWORKS_PATH}/QtCore.framework/Versions/5/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/5/QtCore ${plugin}
  install_name_tool -change ${FRAMEWORKS_PATH}/QtSvg.framework/Versions/5/QtSvg @executable_path/../Frameworks/QtSvg.framework/Versions/5/QtSvg ${plugin}
  install_name_tool -change ${FRAMEWORKS_PATH}/QtWidgets.framework/Versions/5/QtWidgets @executable_path/../Frameworks/QtWidgets.framework/Versions/5/QtWidgets ${plugin}
  # TODO: make these more generic
  install_name_tool -change ${LIB_DIR}/libjpeg.9.dylib @executable_path/../Frameworks/libjpeg.9.dylib ${plugin}
  install_name_tool -change ${LIB_DIR}/libmng.1.dylib  @executable_path/../Frameworks/libmng.1.dylib  ${plugin}
  install_name_tool -change ${LIB_DIR}/libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib ${plugin}
done

# also include some additional image processing libraries
# TODO: make this more generic -> use src files properties ?
NATIVE_IMG_LIBS="\
  libjpeg.9.dylib \
  libmng.1.dylib  \
  libtiff.5.dylib \
  liblcms.1.dylib \
  liblzma.5.dylib"

# copy and fix id and references in these addition libraries
for lib in ${NATIVE_IMG_LIBS}; do
  cp ${LIB_DIR}/${lib} Contents/Frameworks/
  install_name_tool -id @executable_path/../../${lib} Contents/Frameworks/${lib}
  # TODO: make these more generic
  install_name_tool -change ${LIB_DIR}/libjpeg.9.dylib @executable_path/../Frameworks/libjpeg.9.dylib Contents/Frameworks/${lib}
  install_name_tool -change ${LIB_DIR}/libmng.1.dylib  @executable_path/../Frameworks/libmng.1.dylib  Contents/Frameworks/${lib}
  install_name_tool -change ${LIB_DIR}/libtiff.5.dylib @executable_path/../Frameworks/libtiff.5.dylib Contents/Frameworks/${lib}
  install_name_tool -change ${LIB_DIR}/liblcms.1.dylib @executable_path/../Frameworks/liblcms.1.dylib Contents/Frameworks/${lib}
  install_name_tool -change ${LIB_DIR}/liblzma.5.dylib @executable_path/../Frameworks/liblzma.5.dylib Contents/Frameworks/${lib}
  install_name_tool -change ${LIB_DIR}/libz.1.dylib    @executable_path/../Frameworks/libz.1.dylib    Contents/Frameworks/${lib}
done

# this test should stay at the end of this script
# it checks that all shipped dylibs don't contain references to e.g. ${LIB_PREFIX}
HITS=$(find . -name "*.dylib" | xargs otool -L | grep ${LIB_DIR})
if [ $? == 0 ]; then
  echo "WARNING: references to local libs in included libraries...";
  echo ${HITS};
fi
