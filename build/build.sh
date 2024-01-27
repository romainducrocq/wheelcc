#!/bin/bash

cmake -G "Unix Makefiles" -S . -B out/ -DCMAKE_BUILD_TYPE=Release -D__NDEBUG__=ON
cd out/
make

exit 0
