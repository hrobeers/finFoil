#!/bin/bash

# adaptation of https://gist.github.com/asmaloney/55d96a8c3558b2f92cb3

APP=$1
VERSION=$2
ICNS=$3

BUNDLE=$(basename ${APP})
NAME=${BUNDLE%.*}-${VERSION}
DMG=${NAME}.dmg

TEMP_DMG=temp.dmg

# step 1: create staging dir
rm -rf stage
mkdir stage
cp -r ${APP} stage/

# step 2: add Applications link and background resource
cd stage/
ln -s /Applications .
mkdir .background
cp ../../src/resources/background.png .background/background.png
# TODO: temp fix for bad order of copying icns file to app bundle
mkdir -p ${BUNDLE}/Contents/Resources/
cp ${ICNS} ${BUNDLE}/Contents/Resources/
cd ..

# step 3: create new disk image
rm -rf ${TEMP_DMG}
hdiutil create -srcfolder stage/ -volname "${NAME}" -fs HFS+ \
      -fsargs "-c c=64,a=16,e=16" -format UDRW -size 100M "${TEMP_DMG}"

# step 4: mount DMG
DEV=$(hdiutil attach -mountpoint ${NAME} -readwrite -noverify "${TEMP_DMG}" | \
      egrep '^/dev/' | sed 1q | awk '{print $1}')

# step 5: configure DMG

osascript ../scripts/update-dmg.scpt ${NAME} ${BUNDLE}

sync

# step 6: unmount DMG
hdiutil detach ${DEV} -quiet || { echo "failed to unmount ${TEMP_DMG}."; exit 1; }

# step 7: compile final DMG
rm -f ${DMG}
hdiutil convert ${TEMP_DMG} -format UDZO -imagekey zlib-level=9 -o ${DMG}

# step 8: clean up
rm -rf ${NAME}
rm ${TEMP_DMG}
