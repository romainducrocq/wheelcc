#!/bin/bash

ROOT="${PWD}/../.."

function format () {
    FILE="$(readlink -f ${1})"
    cp ${FILE} ./format.out.1

    vi format.out.1 +'e ++ff=dos | set ff=unix | wq!'
    clang-format format.out.1 -style=file > format.out.2
    rm format.out.1

    diff -sq format.out.2 ${FILE} | grep -q " differ"
    if [ ${?} -eq 0 ]; then
        mv format.out.2 ${FILE}
        echo "format ${FILE}"
    else
        rm format.out.2
    fi
}

clang-format --help > /dev/null 2>&1
if [ ${?} -ne 0 ]; then
    sudo apt-get install -y clang-format
fi

if [ ! -z "${1}" ]; then
    format ${1}
else
    for FILE in $(find ${ROOT}/include/ -name "*.h" -type f | sort --uniq); do
        format ${FILE}
    done
    for FILE in $(find ${ROOT}/src/ -name "*.cpp" -type f | sort --uniq); do
        format ${FILE}
    done
    for FILE in $(find ${ROOT}/src/ -name "*.h" -type f | sort --uniq); do
        format ${FILE}
    done
fi

exit 0
