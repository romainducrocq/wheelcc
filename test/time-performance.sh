#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

ROOT="${PWD}/.."
NQCC2="${ROOT}/../nqcc2"

TEST_DIR="${PWD}/tests"
TEST_SRCS=()
for i in $(seq 1 18); do
    TEST_SRCS+=("$(basename $(find ${TEST_DIR} -maxdepth 1 -name "${i}_*" -type d))")
done

function performance () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "--chapter ${1}"
    echo "----------------------------------------------------------------------"
    echo ""
    echo "${PACKAGE_NAME}"
    time for FILE in ${FILES}
    do
        ${PACKAGE_NAME} -S ${FILE} > /dev/null 2>&1
    done
    echo ""
    echo "nqcc2"
    time for FILE in ${FILES}
    do
        ${NQCC2}/_build/default/bin/main.exe -S $(readlink -f ${FILE}) > /dev/null 2>&1
    done
    for FILE in ${FILES}
    do
        FILE=${FILE%.*}.s
        if [ -f ${FILE} ]; then
             rm ${FILE}
        fi
    done
}

function time_src () {
    SRC=${1}
    FILES=$(find ${SRC} -name "*.c" -type f | sort --uniq | grep --invert-match "extra_credit")
    performance ${2}
}

function time_all () {
    i=1
    for SRC in ${TEST_SRCS[@]}
    do
        time_src ${SRC} ${i}
        i=$((i+1))
    done
}

function time_total () {
    FILES=$(find ${TEST_DIR} -name "*.c" -type f | sort --uniq | \
        grep --invert-match "extra_credit" | grep --invert-match "${TEST_DIR}/preprocessor")
    performance "1-${#TEST_SRCS[@]}"
}

cd ${TEST_DIR}
if [ ! "${1}" = "--total" ]; then
    if [ ! -z "${1}" ]; then
        time_src ${TEST_SRCS["$((${1} - 1))"]} ${1}
    else
        time_all
    fi
else
    time_total
fi

exit 0
