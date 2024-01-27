#!/bin/bash

cd ../build/

cmake -G "Unix Makefiles" -S . -B out/ -DCMAKE_BUILD_TYPE=Release
cd out/
make

exit 0
