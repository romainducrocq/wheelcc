#!/bin/bash

PACKAGE_NAME="$(cat ../../bin/package_name.txt)"

TEST_DIR="${PWD}/../tests"
LOG_DIR="${PWD}/../tmp/valgrind"
rm -r ${LOG_DIR}
mkdir -p ${LOG_DIR}

cd ../../bin/

function memcheck () {
    LOG_FILE=$(echo "${2}" | rev | cut -d"." -f2 | rev | tr "/" "_" | cut -d"_" -f3-)

    valgrind --leak-check=full \
             --show-leak-kinds=all \
             --track-origins=yes \
             --verbose \
             --log-file=${LOG_DIR}/${LOG_FILE} \
             ./${PACKAGE_NAME} ${@}

    cat ${LOG_DIR}/${LOG_FILE}
}

RESULT_FILE="result.txt"

CHECK=0
echo -n "" > ${LOG_DIR}/${RESULT_FILE}
for FILE in $(find ${TEST_DIR}/*/valid/ -name "*.c" -type f)
do
    CHECK=$((CHECK+1))
    echo ${CHECK}
    echo -n "${FILE} " >> ${LOG_DIR}/${RESULT_FILE}
    echo "${LOG_FILE}" >> ${LOG_DIR}/${RESULT_FILE}
    memcheck 1 ${FILE} |\
        grep -e "OK" -e "ERROR SUMMARY" >> ${LOG_DIR}/${RESULT_FILE}
    echo "" >> ${LOG_DIR}/${RESULT_FILE}
done
