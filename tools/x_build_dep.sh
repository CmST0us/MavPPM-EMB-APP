#!/bin/bash
CURRENT_DIR=$(cd "$(dirname "${BASH_SOURCE-$0}")"; pwd)
SRC_ROOT=$CURRENT_DIR/..
MAVPPM_ROOT=$SRC_ROOT/..

mkdir -p $MAVPPM_ROOT/Vendor/build/lib
mkdir -p $MAVPPM_ROOT/Vendor/build/include

# Build Socket Kit
cd $MAVPPM_ROOT/Vendor/SocketKit/tools
./x_build_libplist.sh
cd ..
rm -r build
mkdir build
cd build
cmake .. -DBUILD_USBMUXD_COMM=ON -DBUILD_CROSS_COMPILE=ON -DDEBUG=OFF -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
make
make install

# Copy
mkdir -p $MAVPPM_ROOT/Vendor/build/lib
mkdir -p $MAVPPM_ROOT/Vendor/build/include
cp -r libSocketKit/include $MAVPPM_ROOT/Vendor/build
cp -r libSocketKit/lib $MAVPPM_ROOT/Vendor/build
cd ..
cp -r vendor/build/include $MAVPPM_ROOT/Vendor/build
cp -r vendor/build/lib $MAVPPM_ROOT/Vendor/build

# Build PPM
cd $MAVPPM_ROOT/MavPPM-EMB-PPM/tools
./build_wiringPi.sh
cd ..
mkdir -p build
cd build
cmake .. -DBUILD_WIRING_PI=ON -DDEBUG=OFF -DCMAKE_C_COMPILER=arm-linux-gnueabihf-gcc -DCMAKE_CXX_COMPILER=arm-linux-gnueabihf-g++
make
make install

# Copy
cd ..
mkdir -p $MAVPPM_ROOT/Vendor/build/lib
mkdir -p $MAVPPM_ROOT/Vendor/build/include
cp -r vendor/build/include $MAVPPM_ROOT/Vendor/build/
cp -r vendor/build/lib $MAVPPM_ROOT/Vendor/build/
