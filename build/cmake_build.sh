#!/bin/bash

./build_preset.sh
if [ ${?} -ne 0 ]; then exit 1; fi

BUILD_CPP="OFF"
export CC=$(which gcc)
if [[ "${1}" = *"-cpp" ]]; then
    BUILD_CPP="ON"
    export CXX=$(which g++)
fi
if [[ "$(uname -s)" = "Darwin"* ]]; then
    export CC=$(which clang)
    if [ "${BUILD_CPP}" = "ON" ]; then
        export CXX=$(which clang++)
    fi
fi

cmake -G "Unix Makefiles" -S . -B release/ -DCMAKE_BUILD_TYPE=Release \
    -D__NDEBUG__=ON -DBUILD_CPP=${BUILD_CPP}
if [ ${?} -ne 0 ]; then exit 1; fi

cd release/
cmake --build . --config Release
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
