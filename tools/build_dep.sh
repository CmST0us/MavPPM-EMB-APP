#!/bin/bash
CURRENT_DIR=`pwd`
SRC_ROOT=$CURRENT_DIR/..

MAVPPM_ROOT=$SRC_ROOT/..

# Build Socket Kit
cd $MAVPPM_ROOT/Vendor/SocketKit/
rm -r build
mkdir build
cd build
cmake .. -DBUILD_USBMUXD_COMM=ON -DDEBUG=OFF
make
make install

# Copy
mkdir -p $MAVPPM_ROOT/Vendor/build
cp -r libSocketKit/include $MAVPPM_ROOT/Vendor/build
cp -r libSocketKit/lib $MAVPPM_ROOT/Vendor/build

# Build PPM
cd $MAVPPM_ROOT/MavPPM-EMB-PPM
mkdir -p build
cd build
cmake .. -DBUILD_WIRING_PI=ON -DDEBUG=OFF
make
make install

