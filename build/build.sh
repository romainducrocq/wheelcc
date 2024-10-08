#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

if [ -f "../bin/${PACKAGE_NAME}" ]; then
    rm ../bin/${PACKAGE_NAME}
fi

cmake -G "Unix Makefiles" -S . -B release/ -DCMAKE_BUILD_TYPE=Release -D__NDEBUG__=ON
if [ ${?} -ne 0 ]; then exit 1; fi

cd release/
cmake --build . --config Release
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
