#!/bin/bash

FILE=${1%.*}

gcc -pedantic-errors ${FILE}.c -o main > /dev/null 2>&1
./main
RES=${?}
rm main

while true
do
    ccc2 -v ${FILE}.c > ${FILE}.log
    ${FILE}
    RET=${?}
    if [ ${RET} -ne ${RES} ]
    then
        echo ${RET}
        DIR="/home/romain/proj/writing-a-c-compiler/LANG-CCC2/test/tmp/tmp/$(basename "${FILE}")/$(date +%Y%m%d_%H%M%S)"
        mkdir -p ${DIR}
        cp ${FILE}.log ${FILE}.s ${FILE} ${DIR}
        break
    fi
done
