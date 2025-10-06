#!/bin/bash

PACKAGE_DIR="$(dirname $(readlink -f ${0}))"
cd $(dirname ${PACKAGE_DIR})/build/

if [[ "${1}" = "--cmake"* ]]; then
    ./cmake_build.sh ${1}
    if [ ${?} -ne 0 ]; then
        echo -e "\033[0;31merror:\033[0m build failed" 1>&2
        exit 1
    fi
else
    ./build.sh
    if [ ${?} -ne 0 ]; then
        echo -e "\033[0;31merror:\033[0m build failed" 1>&2
        exit 1
    fi
fi

if [ -f "./build_libc.sh" ]; then
    ./build_libc.sh
    if [ ${?} -ne 0 ]; then
        echo -e "\033[0;31merror:\033[0m build libc failed" 1>&2
        exit 1
    fi
fi

echo -e "build was successful, install with \033[1m‘./install.sh’\033[0m"
exit 0
