#!/bin/bash

PACKAGE_TEST="$(dirname $(dirname $(readlink -f ${0})))"
PACKAGE_DIR="$(dirname ${PACKAGE_TEST})/bin"
PACKAGE_NAME="$(cat ${PACKAGE_DIR}/pkgname.cfg)"

EXT_IN="c"
TEST_DIR="${PACKAGE_TEST}/tests/compiler"
if [ -f "${PACKAGE_DIR}/fileext.cfg" ]; then
    EXT_IN="$(cat ${PACKAGE_DIR}/fileext.cfg)"
fi
if [ -f "${PACKAGE_DIR}/pkgpath.cfg" ]; then
    TEST_DIR="$(cat ${PACKAGE_DIR}/pkgpath.cfg)/test/tests/compiler"
fi

function print_errors () {
    for FILE in $(find ${TEST_DIR}/${1}_* -name "*.${EXT_IN}" -type f | grep invalid | sort --uniq); do
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
