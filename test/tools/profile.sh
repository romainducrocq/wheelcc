#!/bin/bash

PACKAGE_NAME="$(cat ../../bin/package_name.txt)"

ROOT="${PWD}/../.."

valgrind --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    sudo apt-get install -y valgrind
fi

ARG=${1}

OPTIM="0 0"
if [ "${1}" = "-O0" ]; then
    ARG=${2}
elif [ "${1}" = "-O1" ]; then
    OPTIM="15 0"
    ARG=${2}
elif [ "${1}" = "-O2" ]; then
    OPTIM="0 2"
    ARG=${2}
elif [ "${1}" = "-O3" ]; then
    OPTIM="15 2"
    ARG=${2}
fi

if [ -z "${ARG}" ]; then exit 1; fi
FILE="$(readlink -f ${ARG%.*})"
if [ ! -f "${FILE}.c" ]; then exit 1; fi

valgrind \
    --tool=callgrind \
    --callgrind-out-file=$(basename ${FILE}).callgrind.out.1 \
    ${ROOT}/bin/${PACKAGE_NAME} 0 ${OPTIM} ${FILE}.c $(dirname ${FILE})/ > /dev/null 2>&1

exit 0

# kcachegrind $(basename ${FILE}).callgrind.out.1
# https://sourceforge.net/projects/qcachegrindwin/
