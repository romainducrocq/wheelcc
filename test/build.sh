#!/bin/bash

cd ../build/

cmake -G "Unix Makefiles" -S . -B test/ -DCMAKE_BUILD_TYPE=Release
cd test/
cmake --build . --config Release

exit 0
