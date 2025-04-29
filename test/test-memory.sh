#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

TEST_DIR="${PWD}/tests/compiler"
TEST_SRCS=()
for i in $(seq 1 20); do
    TEST_SRCS+=("$(basename $(find ${TEST_DIR} -maxdepth 1 -name "${i}_*" -type d))")
done

function file () {
    FILE=${1%.*}
    if [ -f "${FILE}" ]; then rm ${FILE}; fi
    echo "${FILE}"
}

function total () {
    echo "----------------------------------------------------------------------"
    RESULT="${PASS} / ${TOTAL}"
    if [ ${PASS} -eq ${TOTAL} ]; then
        RESULT="${LIGHT_GREEN}PASS: ${RESULT}${NC}"
        RETURN=0
    else
        RESULT="${LIGHT_RED}FAIL: ${RESULT}${NC}"
        RETURN=1
    fi
    echo -e "${RESULT}"
}

function print_check () {
    echo " -(${OPTIM}) check ${1} -> ${2}"
}

function print_memory () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.c${NC}"
    PRINT="$(echo "${SUMMARY}" | cut -d" " -f2-)"
    print_check "memory leaks" "[${PRINT}]"
}

function check_memory () {
    INCLUDE_DIR="$(dirname ${TEST_DIR}/${FILE}.c)/"

    let TOTAL+=1

    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --log-file=valgrind.out.1 \
             ./${PACKAGE_NAME} 0 ${OPTIM} ${TEST_DIR}/${FILE}.c ${INCLUDE_DIR} > /dev/null 2>&1

    SUMMARY=$(cat valgrind.out.1 | \
        grep -e "ERROR SUMMARY" \
             -e "HEAP SUMMARY" \
             -e "in use at exit" \
             -e "total heap usage" \
             -e "All heap blocks were freed -- no leaks are possible")
    MEMCHECK=1
    echo "${SUMMARY}" | grep -q "ERROR SUMMARY: 0 errors from 0 contexts"
    if [ ${?} -eq 0 ]; then
        echo "${SUMMARY}" | grep -q "in use at exit: 0 bytes in 0 blocks"
        if [ ${?} -eq 0 ]; then
            echo "${SUMMARY}" | grep -q "All heap blocks were freed -- no leaks are possible"
            if [ ${?} -eq 0 ]; then
                MEMCHECK=0
            fi
        fi
    fi
    if [ ${MEMCHECK} -eq 0 ]; then
        RESULT="${LIGHT_GREEN}[y]"
        let PASS+=1
    else
        RESULT="${LIGHT_RED}[n]"
    fi
    rm valgrind.out.1

    print_memory
}

function check_test () {
    FILE=$(file ${1})
    cd ../../../bin/
    check_memory
    cd ${TEST_DIR}
}

function test_src () {
    SRC=${1}
    for FILE in $(find ${SRC} -name "*.c" -type f | sort --uniq); do
        check_test ${FILE}
    done
}

function test_all () {
    for SRC in ${TEST_SRCS[@]}; do
        test_src ${SRC}
    done
}

DISTRO="$(cat /etc/os-release | grep -P "^NAME=" | cut -d"\"" -f2)"
case ${DISTRO} in
    "Arch Linux") ;&
    "EndeavourOS") ;&
    "Manjaro")
        export DEBUGINFOD_URLS="https://debuginfod.archlinux.org"
esac

PASS=0
TOTAL=0
RETURN=0

ARG=${1}

OPTIM="0 0"
if [ "${1}" = "-O0" ]; then
    ARG=${2}
elif [ "${1}" = "-O1" ]; then
    OPTIM="15 0"
    ARG=${2}
elif [ "${1}" = "-O2" ]; then
    OPTIM="0 2"
    ARG=${2}
elif [ "${1}" = "-O3" ]; then
    OPTIM="15 2"
    ARG=${2}
fi

cd ${TEST_DIR}
if [ ! -z "${ARG}" ]; then
    test_src ${TEST_SRCS["$((${ARG} - 1))"]}
else
    test_all
fi
total

exit ${RETURN}
