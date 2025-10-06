#!/bin/bash

PACKAGE_TEST="$(dirname $(dirname $(readlink -f ${0})))"
PACKAGE_DIR="$(dirname ${PACKAGE_TEST})/bin"
PACKAGE_NAME="$(cat ${PACKAGE_DIR}/pkgname.cfg)"

EXT_IN="c"
if [ -f "${PACKAGE_DIR}/fileext.cfg" ]; then
    EXT_IN="$(cat ${PACKAGE_DIR}/fileext.cfg)"
fi

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
if [ ! -f "${FILE}.${EXT_IN}" ]; then exit 1; fi

valgrind \
    --tool=callgrind \
    --callgrind-out-file=$(basename ${FILE}).callgrind.out.1 \
    ${PACKAGE_DIR}/${PACKAGE_NAME} 0 ${OPTIM} ${FILE}.${EXT_IN} ${PACKAGE_DIR}/libc/ $(dirname ${FILE})/ > /dev/null 2>&1

exit 0

# kcachegrind $(basename ${FILE}).callgrind.out.1
# https://sourceforge.net/projects/qcachegrindwin/
