#!/bin/bash

PACKAGE_NAME="$(cat ../../bin/package_name.txt)"

TEST_DIR="${PWD}/../tests/compiler"

function print_errors () {
    for FILE in $(find ${TEST_DIR}/${1}_* -name "*.c" -type f | grep invalid | sort --uniq); do
        if [ ! -z "${MATCH_PATTERN}" ]; then
            cat <(${PACKAGE_NAME} -s ${FILE} 2>&1) | grep -P "${MATCH_PATTERN}"
        else
            cat <(${PACKAGE_NAME} -s ${FILE} 2>&1)
        fi
        if [ -f ${FILE%.*}.s ]; then
            rm ${FILE%.*}.s
        fi
    done
}

ARG=${1}

MATCH_PATTERN=""
if [ "${1}" = "--what" ]; then
    MATCH_PATTERN="(no\. [0-9]+)"
    ARG=${2}
elif [ "${1}" = "--line" ]; then
    MATCH_PATTERN="^at line [0-9]+:"
    ARG=${2}
fi

if [[ "$(uname -s)" = "Darwin"* ]]; then
    MATCH_PATTERN=""
fi

if [ ! -z "${ARG}" ]; then
    print_errors ${ARG}
else
    for i in $(seq 1 20); do
        print_errors ${i}
    done
fi

exit 0
