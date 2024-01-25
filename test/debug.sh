#!/bin/bash

cd ../build/

cmake -G "Unix Makefiles" -S . -B out/ -DCMAKE_BUILD_TYPE=Debug
cd out/
make

exit 0
