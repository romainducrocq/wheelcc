#!/bin/bash

STAGE="${1}"
cd ../compiler
for FILE in $(find . -name "*.c" -type f); do
    STDOUT=$(wheelcc --${STAGE} -O0 ${FILE} 2> /dev/null)
    diff ../stages/${FILE%.*}.${STAGE} <(echo "${STDOUT}")
done
