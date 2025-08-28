#!/bin/bash

STAGE="${1}"
if [[ "${1}" = "codegen_no"* ]]; then
    STAGE="codegen"
fi

cd ../compiler
for FILE in $(find . -name "*.c" -type f); do
    STDOUT=$(wheelcc --${STAGE} -O0 ${FILE} 2> /dev/null)
    diff ../stages/${FILE%.*}.${1} <(echo "${STDOUT}")
done
