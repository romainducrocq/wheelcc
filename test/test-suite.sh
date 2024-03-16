#!/bin/bash

ROOT="${PWD}/.."

function test () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "${@}"
    ./test_compiler ${ROOT}/bin/driver.sh ${@}
    if [ ${?} -ne 0 ]; then exit 1; fi
}

cd ${ROOT}/../writing-a-c-compiler-tests/

if [ ${#} -ne 0 ]; then
    test ${@}
else
    for i in $(seq 1 16); do
        test --chapter ${i} --latest-only --bitwise --compound --goto --nan
    done
fi

exit 0
