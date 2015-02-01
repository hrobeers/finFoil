#!/bin/bash

command -v inkscape >/dev/null 2>&1 || { echo >&2 "ERROR: inkscape is required but it's not installed."; exit 1; }

FILE=$1
FILENAME=`basename ${FILE}`
NAME=${FILENAME%.*}

# step 1: create directory and move all PNGs in it
rm -rf ${NAME}.iconset
mkdir ${NAME}.iconset

cd ${NAME}.iconset

# step 2: generate required PNGs from the input SVG provided to the script
for size in 16 32 64 128 256 512 1024; do
  inkscape -z -e icon_${size}x${size}.png -w ${size} -h ${size} ../${FILE}
done

# step 3: duplicate and/or rename PNGs according to specs
cp icon_32x32.png icon_16x16@2x.png
mv icon_64x64.png icon_32x32@2x.png
cp icon_256x256.png icon_128x128@2x.png
cp icon_512x512.png icon_256x256@2x.png
mv icon_1024x1024.png icon_512x512@2x.png

cd ..

# step 4: transform directory into actual iconset
rm -rf ${NAME}.icns
iconutil -c icns ${NAME}.iconset

# step 5: clean up temporary iconset folder
rm -rf ${NAME}.iconset