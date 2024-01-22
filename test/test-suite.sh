#!/bin/bash

function test () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "${@}"
    ./test_compiler ../LANG-CCC2/bin/driver.sh ${@}
    if [ ${?} -ne 0 ]; then exit 1; fi
}

cd ../../writing-a-c-compiler-tests/

if [ ${#} -ne 0 ]; then
    test ${@}
else
    for i in $(seq 1 13); do
        test --chapter ${i} --stage lex --latest-only --bitwise --compound --goto --nan
    done
fi

exit 0