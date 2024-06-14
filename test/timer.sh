#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

ROOT="${PWD}/.."
NQCC2="${ROOT}/../nqcc2"

function timer () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "--chapter ${1}"
    echo "----------------------------------------------------------------------"
    echo ""
    echo "${PACKAGE_NAME}"
    time for FILE in $(find tests/${1}_* -name "*.c" -type f)
    do
        ${PACKAGE_NAME} -S ${FILE} > /dev/null 2>&1
    done
    echo ""
    echo "nqcc2"
    time for FILE in $(find tests/${1}_* -name "*.c" -type f)
    do
        ${NQCC2}/_build/default/bin/main.exe -S $(readlink -f ${FILE}) > /dev/null 2>&1
    done
    for FILE in $(find tests/${1}_* -name "*.c" -type f)
    do
        FILE=${FILE%.*}
        if [ -f ${FILE} ]; then
             rm ${FILE}
        fi
    done
}

if [ ! -z "${1}" ]; then
    timer ${1}
else
    for i in $(seq 1 18); do
        timer ${i}
    done
fi

exit 0
