#!/bin/bash

./build_preset.sh
if [ ${?} -ne 0 ]; then exit 1; fi

export CC=$(which gcc)
export CXX=$(which g++)

cmake -G "Unix Makefiles" -S . -B release/ -DCMAKE_BUILD_TYPE=Release -D__NDEBUG__=ON
if [ ${?} -ne 0 ]; then exit 1; fi

cd release/
cmake --build . --config Release
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
