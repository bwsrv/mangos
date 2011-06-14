#!/bin/sh
PREFIX=${PWD}/bin
CONF_OPTS="-DPCH=1 -DDEBUG=1"
FLAGS="-march=native -O2 -pipe -DNDEBUG"

rm -Rf build &&
mkdir build &&
cd build &&
cmake .. -DPREFIX="${PREFIX}" ${CONF_OPTS} -DCMAKE_C_FLAGS="${FLAGS}" -DCMAKE_CXX_FLAGS="${FLAGS}" -DCMAKE_C_COMPILER="gcc" -DCMAKE_CXX_COMPILER="g++"
