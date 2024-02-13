#!/bin/bash

mkdir -p tmp/debug/

TIME="$(date +%Y%m%d_%H%M%S)"
DEBUG_FILE="$(pwd)/tmp/debug_${TIME}.txt"
touch ${DEBUG_FILE}
echo "${DEBUG_FILE}"

cd ../
for i in $(seq 1 2);
do
    echo "-- test ${i}" | tee -a ${DEBUG_FILE}
    ./test.sh | grep "\[n\]" | tee -a ${DEBUG_FILE}
done
