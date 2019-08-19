#!/bin/bash -x

rm -rf build
mkdir build
cd build
cmake -DPYTHON_VER=3 -DDEBUG=on -DCMAKE_EXPORT_COMPILE_COMMANDS=on -DTESTS=on ..
make install
