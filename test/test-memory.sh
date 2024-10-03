#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

TEST_DIR="${PWD}/tests/compiler"
TEST_SRCS=()
for i in $(seq 1 18); do
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
    else
        RESULT="${LIGHT_RED}FAIL: ${RESULT}${NC}"
    fi
    echo -e "${RESULT}"
}

function print_check () {
    echo " - check ${1} -> ${2}"
}

function print_memory () {
    echo -e -n "${TOTAL} ${RESULT} ${FILE}.c${NC}"
    PRINT="$(echo "${SUMMARY}" | cut -d" " -f2-)"
    print_check "memory leaks" "[${PRINT}]"
}

function check_memory () {
    INCLUDE_DIR="$(dirname ${TEST_DIR}/${FILE}.c)/"
    if ! ( ./${PACKAGE_NAME} 0 30 2 ${TEST_DIR}/${FILE}.c ${INCLUDE_DIR} || false ) > /dev/null 2>&1; then
        return
    fi

    let TOTAL+=1

    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --log-file=valgrind.out.1 \
             ./${PACKAGE_NAME} 0 30 2 ${TEST_DIR}/${FILE}.c ${INCLUDE_DIR}

    SUMMARY=$(cat valgrind.out.1 | grep "ERROR SUMMARY")
    echo "${SUMMARY}" | grep -q "ERROR SUMMARY: 0 errors"
    if [ ${?} -eq 0 ]; then
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
    for FILE in $(find ${SRC} -name "*.c" -type f | sort --uniq)
    do
        check_test ${FILE}
    done
}

function test_all () {
    for SRC in ${TEST_SRCS[@]}
    do
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
cd ${TEST_DIR}
if [ ! -z "${1}" ]; then
    test_src ${TEST_SRCS["$((${1} - 1))"]}
else
    test_all
fi
total

exit 0
