#!/bin/bash

PACKAGE_NAME="$(cat ../../bin/pkgname.cfg)"

ROOT="${PWD}/../.."
NQCC2="${ROOT}/../nqcc2"

TEST_DIR="${PWD}/../tests/compiler"
TEST_SRCS=()
for i in $(seq 1 20); do
    TEST_SRCS+=("$(basename $(find ${TEST_DIR} -maxdepth 1 -name "${i}_*" -type d))")
done

function performance () {
    echo ""
    echo "----------------------------------------------------------------------"
    echo "--test ${1}"
    echo "----------------------------------------------------------------------"
    echo ""
    echo "${PACKAGE_NAME} ${OPTIM}"
    { time for FILE in ${FILES}; do
        ${PACKAGE_NAME} ${OPTIM} -S ${FILE} > /dev/null 2>&1
    done } 2>&1
    if [ 1 -eq 0 ]; then
        echo ""
        echo "nqcc2 ${OPTIM_NQCC2}"
        { time for FILE in ${FILES}; do
            ${NQCC2}/_build/default/bin/main.exe ${OPTIM_NQCC2} -S $(readlink -f ${FILE}) > /dev/null 2>&1
        done } 2>&1
    fi
    for FILE in ${FILES}; do
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
    for SRC in ${TEST_SRCS[@]}; do
        time_src ${SRC} ${i}
        i=$((i+1))
    done
}

function time_total () {
    FILES=$(find ${TEST_DIR} -name "*.c" -type f | sort --uniq | grep --invert-match "extra_credit")
    performance "1-${#TEST_SRCS[@]}"
}

ARG=${1}

OPTIM="-O0"
OPTIM_NQCC2=""
if [ "${1}" = "-O0" ]; then
    ARG=${2}
elif [ "${1}" = "-O1" ]; then
    OPTIM="-O0 -O1"
    OPTIM_NQCC2="--optimize"
    ARG=${2}
elif [ "${1}" = "-O2" ]; then
    OPTIM="-O0 -O2"
    OPTIM_NQCC2=""
    ARG=${2}
elif [ "${1}" = "-O3" ]; then
    OPTIM="-O3"
    OPTIM_NQCC2="--optimize"
    ARG=${2}
fi

cd ${TEST_DIR}
if [ ! "${ARG}" = "--total" ]; then
    if [ ! -z "${ARG}" ]; then
        time_src ${TEST_SRCS["$((${ARG} - 1))"]} ${ARG}
    else
        time_all
    fi
else
    time_total
fi

exit 0
