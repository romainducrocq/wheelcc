#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

ROOT="${PWD}/.."

if [ -z "${1}" ]; then exit 1; fi
FILE="$(readlink -f ${1%.*})"
if [ ! -f "${FILE}.c" ]; then exit 1; fi

valgrind \
    --tool=callgrind \
    --callgrind-out-file=$(basename ${FILE}).callgrind.out.1 \
    ${ROOT}/bin/${PACKAGE_NAME} 0 ${FILE}.c $(dirname ${FILE})/ > /dev/null 2>&1

exit 0

# kcachegrind $(basename ${FILE}).callgrind.out.1
# https://sourceforge.net/projects/qcachegrindwin/
