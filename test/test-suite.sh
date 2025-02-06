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
        if [ ${i} -le 18 ]; then
            test --chapter ${i} --latest-only --extra-credit
        fi
#        test --chapter ${i} --latest-only --fold-constants --extra-credit
#        test --chapter ${i} --latest-only --eliminate-unreachable-code --extra-credit
        test --chapter ${i} --latest-only --propagate-copies --extra-credit
        if [ ${i} -le 18 ]; then
            if [ ${i} -gt 10 ]; then
                continue
            fi
        fi
        test --chapter ${i} --latest-only --eliminate-dead-stores --extra-credit --int-only
    done
fi

exit 0
