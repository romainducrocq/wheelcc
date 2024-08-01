#!/bin/bash

PACKAGE_NAME="$(cat ../bin/package_name.txt)"

LIGHT_RED='\033[1;31m'
LIGHT_GREEN='\033[1;32m'
NC='\033[0m'

TEST_DIR="${PWD}/tests"
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
    RES="${PASS} / ${TOTAL}"
    if [ ${PASS} -eq ${TOTAL} ]; then
        RES="${LIGHT_GREEN}PASS: ${RES}${NC}"
    else
        RES="${LIGHT_RED}FAIL: ${RES}${NC}"
    fi
    echo -e "${RES}"
}

function print_check () {
    echo " - check ${1} -> ${2}"
}

function print_memory () {
    echo -e -n "${TOTAL} ${RES} ${FILE}.c${NC}"
    PRINT="$(echo "${SUMMARY}" | cut -d" " -f2-)"
    print_check "memory leaks" "[${PRINT}]"
}

function check_memory () {
    INCLUDE_DIR="$(dirname ${TEST_DIR}/${FILE}.c)/"
    if ! ( ./${PACKAGE_NAME} 0 ${TEST_DIR}/${FILE}.c ${INCLUDE_DIR} || false ) > /dev/null 2>&1; then
        return
    fi

    let TOTAL+=1

    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --log-file=valgrind.out.1 \
             ./${PACKAGE_NAME} 0 ${TEST_DIR}/${FILE}.c ${INCLUDE_DIR}

    SUMMARY=$(cat valgrind.out.1 | grep "ERROR SUMMARY")
    echo "${SUMMARY}" | grep -q "ERROR SUMMARY: 0 errors"
    if [ ${?} -eq 0 ]; then
        RES="${LIGHT_GREEN}[y]"
        let PASS+=1
    else
        RES="${LIGHT_RED}[n]"
    fi
    rm valgrind.out.1

    print_memory
}

function check_test () {
    FILE=$(file ${1})
    cd ../../bin/
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

dpkg -s valgrind | grep Status | grep -q "install ok installed"
if [ ${?} -ne 0 ]; then
    sudo apt-get install -y valgrind
fi

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
