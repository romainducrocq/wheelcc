#!/bin/bash

cd ../

DATE="$(date +%Y%m%d_%H%M%S)"
TESTS_DIR="tests"
TMP_DIR="debug/tmp"
TMP_FILE="${TMP_DIR}/tmp"
FAIL_DIR="${TMP_DIR}/fail"
FAIL_FILE="${FAIL_DIR}/_0_results_${DATE}.txt"

mkdir -p ${FAIL_DIR}
touch ${FAIL_FILE}
echo "${FAIL_FILE}"

function duration () {
  DURATION=${SECONDS}
  echo "$((DURATION / 3600))h $(($((DURATION % 3600)) / 60))min $((DURATION % 60))sec"
}
SECONDS=0

N=100
for i in $(seq 1 ${N})
do
    DATE="$(date +%Y%m%d_%H%M%S)"
    echo -n "" > ${TMP_FILE}
    echo "-- test $((i-1)) / ${N} ($(duration))" | tee -a ${FAIL_FILE}
    ./test.sh | grep "\[n\]" | tee -a ${FAIL_FILE} ${TMP_FILE}
    for FILE_C in $(cat ${TMP_FILE} | cut -d" " -f3)
    do
        FILE=${FILE_C%.*}
        cp ${TESTS_DIR}/${FILE}.s \
           ${FAIL_DIR}/$(echo ${FILE} | tr "/" "_")_${DATE}.s
    done
    rm ${TMP_FILE}

    find ${TESTS_DIR} -name "*.s" -type f -delete
done
echo "-- test ${N} / ${N} ($(duration))" | tee -a ${FAIL_FILE}
