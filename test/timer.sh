#!/bin/bash

for i in $(seq 1 17)
do
    echo ""
    echo "%%${i} ------------------------------------------------"
    time for FILE in $(find tests/${i}_* -name "*.c" -type f)
    do
        wheelcc ${FILE} > /dev/null 2>&1
    done
    time for FILE in $(find tests/${i}_* -name "*.c" -type f)
    do
        ../../nqcc2/nqcc2/_build/default/bin/main.exe $(readlink -f ${FILE}) > /dev/null 2>&1
    done
done
