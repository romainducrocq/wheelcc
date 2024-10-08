#!/bin/bash

ROOT="${PWD}/.."
TEST_SUITE="${ROOT}/../writing-a-c-compiler-tests"

function test () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "${@}"
    ./test_compiler ${ROOT}/bin/driver.sh ${@}
    if [ ${?} -ne 0 ]; then exit 1; fi
}

cd ${TEST_SUITE}

if [ ${#} -ne 0 ]; then
    test ${@}
else
    for i in $(seq 1 19); do
        if [ ${i} -ne 19 ]; then if [ ${i} -gt 10 ]; then continue; fi; fi
        test --chapter ${i} --latest-only --extra-credit --fold-constants --int-only
    done
fi

exit 0
