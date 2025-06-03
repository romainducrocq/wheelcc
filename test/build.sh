#!/bin/bash

cd ../build/

if [[ "$(uname -s)" = "Darwin"* ]]; then
    ./build.sh
    exit ${?}
fi

./build_preset.sh
if [ ${?} -ne 0 ]; then exit 1; fi

export CC=$(which gcc)
export CXX=$(which g++)

cmake -G "Unix Makefiles" -S . -B test/ -DCMAKE_BUILD_TYPE=Release
if [ ${?} -ne 0 ]; then exit 1; fi

cd test/
cmake --build . --config Release
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
