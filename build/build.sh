#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

if [ -f "../bin/${PACKAGE_NAME}" ]; then
    rm ../bin/${PACKAGE_NAME}
fi

GCC_MAJOR_VERSION=$(gcc -dumpversion | cut -d"." -f1)
if [ ${GCC_MAJOR_VERSION} -lt 8 ]; then
    echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m requires \033[1m‘gcc’\033[0m >= 8.1.0" 1>&2
    exit 1
elif [ ${GCC_MAJOR_VERSION} -eq 8 ]; then
    GCC_MINOR_VERSION=$(gcc -dumpfullversion | cut -d"." -f2)
    if [ ${GCC_MINOR_VERSION} -eq 0 ]; then
        echo -e "${PACKAGE_NAME}: \033[0;31merror:\033[0m requires \033[1m‘gcc’\033[0m >= 8.1.0" 1>&2
        exit 1
    fi
fi

export CC=$(which gcc)
export CXX=$(which g++)

cmake -G "Unix Makefiles" -S . -B release/ -DCMAKE_BUILD_TYPE=Release -D__NDEBUG__=ON
if [ ${?} -ne 0 ]; then exit 1; fi

cd release/
cmake --build . --config Release
if [ ${?} -ne 0 ]; then exit 1; fi

exit 0
