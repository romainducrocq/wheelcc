#!/bin/bash

RESULTS_DIR="${PWD}/test_results/"
mkdir -p ${RESULTS_DIR}
RESULTS_DIR="$(echo "${RESULTS_DIR}$(find ${RESULTS_DIR} -type d | wc -l)" | tr -d ' ')"
if [ -d "${RESULTS_DIR}" ]; then rm -r "${RESULTS_DIR}"; fi
mkdir ${RESULTS_DIR}

echo "Log results to: ${RESULTS_DIR}/"
echo ""

function test () {
    echo "./${@}"
    RESULTS_FILE="${RESULTS_DIR}/${1%.*}${2}.out.1"
    ./${@} > ${RESULTS_FILE}
    RETURN=${?}
    cat ${RESULTS_FILE} | tail -1
    echo ""
    if [ ${RETURN} -ne 0 ]; then exit 1; fi
}

test "test-preprocessor.sh"

test "test-compiler.sh" "-O0"
test "test-compiler.sh" "-O1"
test "test-compiler.sh" "-O2"
test "test-compiler.sh" "-O3"

test "test-memory.sh" "-O0"
test "test-memory.sh" "-O1"
test "test-memory.sh" "-O2"
test "test-memory.sh" "-O3"

test "test-errors.sh"

exit 0
