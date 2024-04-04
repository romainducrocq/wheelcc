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
    for i in $(seq 1 18); do
        if [ ${i} -eq 5 ]; then continue; fi
        test --chapter ${i} --latest-only --bitwise --compound --goto --nan \
             --stage parse
    done
fi

exit 0
