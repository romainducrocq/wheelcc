#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

cd ../build/

if [ -f "../bin/${PACKAGE_NAME}" ]; then
    rm ../bin/${PACKAGE_NAME}
fi

cmake -G "Unix Makefiles" -S . -B test/ -DCMAKE_BUILD_TYPE=Release
cd test/
cmake --build . --config Release

exit 0
