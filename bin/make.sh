#!/usr/bin/env bash

PACKAGE_DIR="$(dirname $(readlink -f ${0}))"
cd $(dirname ${PACKAGE_DIR})/build/

function help () {
    echo "Usage: ./make.sh [--cmake | --cmake-cpp]"
    exit 0
}

function em () {
    echo "\033[1m‘${1}’\033[0m"
}

function raise_error () {
    ERROR_MESSAGE="${1}"
    echo -e "make: \033[0;31merror:\033[0m ${ERROR_MESSAGE}" 1>&2
    exit 1
}

if [ -z "${1}" ]; then
    ./build.sh
    if [ ${?} -ne 0 ]; then
        raise_error "build failed"
    fi
elif [[ "${1}" == "--cmake"* ]]; then
    ./cmake_build.sh ${1}
    if [ ${?} -ne 0 ]; then
        raise_error "cmake build failed"
    fi
else
    help
fi

if [ -f "./build_libc.sh" ]; then
    ./build_libc.sh
    if [ ${?} -ne 0 ]; then
        raise_error "failed to build runtime"
    fi
fi

echo -e "build was successful, install with $(em "./install.sh")"
exit 0
