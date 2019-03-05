#!/bin/bash
CURRENT_DIR=$(cd "$(dirname "${BASH_SOURCE-$0}")"; pwd)
SRC_ROOT=$CURRENT_DIR/..
MAVPPM_ROOT=$SRC_ROOT/..

mkdir -p $MAVPPM_ROOT/Vendor/build/lib
mkdir -p $MAVPPM_ROOT/Vendor/build/include

# Build Socket Kit
cd $MAVPPM_ROOT/Vendor/SocketKit/tools
./build_libplist.sh
cd ..
rm -r build
mkdir build
cd build
cmake .. -DBUILD_USBMUXD_COMM=ON -DBUILD_CROSS_COMPILE=OFF -DDEBUG=OFF
make
make install

# Copy
mkdir -p $MAVPPM_ROOT/Vendor/build
cp -r libSocketKit/include $MAVPPM_ROOT/Vendor/build
cp -r libSocketKit/lib $MAVPPM_ROOT/Vendor/build
cd ..
cp -r vendor/build/include $MAVPPM_ROOT/Vendor/build
cp -r vendor/build/lib $MAVPPM_ROOT/Vendor/lib

# Build PPM
cd $MAVPPM_ROOT/MavPPM-EMB-PPM
mkdir -p build
cd build
cmake .. -DBUILD_WIRING_PI=OFF -DDEBUG=OFF
make
make install
