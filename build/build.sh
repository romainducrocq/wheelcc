#!/bin/bash

cmake -G "Unix Makefiles" -S . -B release/ -DCMAKE_BUILD_TYPE=Release -D__NDEBUG__=ON
cd release/
cmake --build . --config Release

exit 0
